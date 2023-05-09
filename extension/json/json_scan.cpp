#include "json_scan.hpp"

#include "duckdb/common/multi_file_reader.hpp"
#include "duckdb/main/extension_helper.hpp"
#include "duckdb/parallel/task_scheduler.hpp"
#include "duckdb/storage/buffer_manager.hpp"

namespace duckdb {

JSONScanData::JSONScanData() {
}

unique_ptr<FunctionData> JSONScanData::Bind(ClientContext &context, TableFunctionBindInput &input) {
	auto result = make_uniq<JSONScanData>();
	auto &options = result->options;

	auto &info = input.info->Cast<JSONScanInfo>();
	result->type = info.type;
	options.format = info.format;
	options.record_type = info.record_type;
	result->auto_detect = info.auto_detect;

	for (auto &kv : input.named_parameters) {
		auto loption = StringUtil::Lower(kv.first);
		if (loption == "ignore_errors") {
			result->ignore_errors = BooleanValue::Get(kv.second);
		} else if (loption == "maximum_object_size") {
			result->maximum_object_size = MaxValue<idx_t>(UIntegerValue::Get(kv.second), result->maximum_object_size);
		} else if (loption == "format") {
			auto arg = StringValue::Get(kv.second);
			if (arg == "auto") {
				options.format = JSONFormat::AUTO_DETECT;
			} else if (arg == "unstructured") {
				options.format = JSONFormat::UNSTRUCTURED;
			} else if (arg == "newline_delimited" || arg == "nd") {
				options.format = JSONFormat::NEWLINE_DELIMITED;
			} else if (arg == "array") {
				options.format = JSONFormat::ARRAY;
			} else {
				throw InvalidInputException(
				    "format must be one of ['auto', 'unstructured', 'newline_delimited', 'array']");
			}
		} else if (loption == "compression") {
			auto compression = StringUtil::Lower(StringValue::Get(kv.second));
			if (compression == "none") {
				options.compression = FileCompressionType::UNCOMPRESSED;
			} else if (compression == "gzip") {
				options.compression = FileCompressionType::GZIP;
			} else if (compression == "zstd") {
				options.compression = FileCompressionType::ZSTD;
			} else if (compression == "auto") {
				options.compression = FileCompressionType::AUTO_DETECT;
			} else {
				throw BinderException("compression must be one of ['none', 'gzip', 'zstd', 'auto']");
			}
		}
	}

	result->files = MultiFileReader::GetFileList(context, input.inputs[0], "JSON");
	result->InitializeReaders(context);

	return std::move(result);
}

void JSONScanData::InitializeReaders(ClientContext &context) {
	union_readers.resize(files.empty() ? 0 : files.size() - 1);
	for (idx_t file_idx = 0; file_idx < files.size(); file_idx++) {
		if (file_idx == 0) {
			initial_reader = make_uniq<BufferedJSONReader>(context, options, files[0]);
		} else {
			union_readers[file_idx - 1] = make_uniq<BufferedJSONReader>(context, options, files[file_idx]);
		}
	}
}

void JSONScanData::InitializeFormats() {
	InitializeFormats(auto_detect);
}

void JSONScanData::InitializeFormats(bool auto_detect_p) {
	// Set defaults for date/timestamp formats if we need to
	if (!auto_detect_p && date_format.empty()) {
		date_format = "%Y-%m-%d";
	}
	if (!auto_detect_p && timestamp_format.empty()) {
		timestamp_format = "%Y-%m-%dT%H:%M:%S.%fZ";
	}

	// Initialize date_format_map if anything was specified
	if (!date_format.empty()) {
		date_format_map.AddFormat(LogicalTypeId::DATE, date_format);
	}
	if (!timestamp_format.empty()) {
		date_format_map.AddFormat(LogicalTypeId::TIMESTAMP, timestamp_format);
	}

	if (auto_detect_p) {
		static const type_id_map_t<vector<const char *>> FORMAT_TEMPLATES = {
		    {LogicalTypeId::DATE, {"%m-%d-%Y", "%m-%d-%y", "%d-%m-%Y", "%d-%m-%y", "%Y-%m-%d", "%y-%m-%d"}},
		    {LogicalTypeId::TIMESTAMP,
		     {"%Y-%m-%d %H:%M:%S.%f", "%m-%d-%Y %I:%M:%S %p", "%m-%d-%y %I:%M:%S %p", "%d-%m-%Y %H:%M:%S",
		      "%d-%m-%y %H:%M:%S", "%Y-%m-%d %H:%M:%S", "%y-%m-%d %H:%M:%S", "%Y-%m-%dT%H:%M:%SZ"}},
		};

		// Populate possible date/timestamp formats, assume this is consistent across columns
		for (auto &kv : FORMAT_TEMPLATES) {
			const auto &logical_type = kv.first;
			if (date_format_map.HasFormats(logical_type)) {
				continue; // Already populated
			}
			const auto &format_strings = kv.second;
			for (auto &format_string : format_strings) {
				date_format_map.AddFormat(logical_type, format_string);
			}
		}
	}
}

void JSONScanData::Serialize(FieldWriter &writer) const {
	writer.WriteField<JSONScanType>(type);

	options.Serialize(writer);
	writer.WriteList<string>(files);

	writer.WriteField<bool>(ignore_errors);
	writer.WriteField<idx_t>(maximum_object_size);
	writer.WriteField<bool>(auto_detect);
	writer.WriteField<idx_t>(sample_size);
	writer.WriteField<idx_t>(max_depth);

	transform_options.Serialize(writer);
	writer.WriteList<string>(names);
	if (!date_format.empty()) {
		writer.WriteString(date_format);
	} else {
		writer.WriteString(date_format_map.GetFormat(LogicalTypeId::DATE).format_specifier);
	}
	if (!timestamp_format.empty()) {
		writer.WriteString(timestamp_format);
	} else {
		writer.WriteString(date_format_map.GetFormat(LogicalTypeId::TIMESTAMP).format_specifier);
	}
}

void JSONScanData::Deserialize(ClientContext &context, FieldReader &reader) {
	type = reader.ReadRequired<JSONScanType>();

	options.Deserialize(reader);
	files = reader.ReadRequiredList<string>();
	InitializeReaders(context);

	ignore_errors = reader.ReadRequired<bool>();
	maximum_object_size = reader.ReadRequired<idx_t>();
	auto_detect = reader.ReadRequired<bool>();
	sample_size = reader.ReadRequired<idx_t>();
	max_depth = reader.ReadRequired<idx_t>();

	transform_options.Deserialize(reader);
	names = reader.ReadRequiredList<string>();
	date_format = reader.ReadRequired<string>();
	timestamp_format = reader.ReadRequired<string>();

	InitializeFormats();
	transform_options.date_format_map = &date_format_map;
}

JSONScanGlobalState::JSONScanGlobalState(ClientContext &context, const JSONScanData &bind_data_p)
    : bind_data(bind_data_p), transform_options(bind_data.transform_options),
      allocator(BufferManager::GetBufferManager(context).GetBufferAllocator()),
      buffer_capacity(bind_data.maximum_object_size * 2), file_index(0), batch_index(0),
      system_threads(TaskScheduler::GetScheduler(context).NumberOfThreads()) {
}

JSONScanLocalState::JSONScanLocalState(ClientContext &context, JSONScanGlobalState &gstate)
    : scan_count(0), batch_index(DConstants::INVALID_INDEX), total_read_size(0), total_tuple_count(0),
      bind_data(gstate.bind_data), allocator(BufferAllocator::Get(context)), current_reader(nullptr),
      current_buffer_handle(nullptr), is_last(false), buffer_size(0), buffer_offset(0), prev_buffer_remainder(0) {

	// Buffer to reconstruct JSON values when they cross a buffer boundary
	reconstruct_buffer = gstate.allocator.Allocate(gstate.bind_data.maximum_object_size + YYJSON_PADDING_SIZE);
}

JSONGlobalTableFunctionState::JSONGlobalTableFunctionState(ClientContext &context, TableFunctionInitInput &input)
    : state(context, input.bind_data->Cast<JSONScanData>()) {
}

unique_ptr<GlobalTableFunctionState> JSONGlobalTableFunctionState::Init(ClientContext &context,
                                                                        TableFunctionInitInput &input) {
	auto &bind_data = input.bind_data->Cast<JSONScanData>();
	auto result = make_uniq<JSONGlobalTableFunctionState>(context, input);
	auto &gstate = result->state;

	// Perform projection pushdown
	if (bind_data.type == JSONScanType::READ_JSON) {
		D_ASSERT(input.column_ids.size() <= bind_data.names.size()); // Can't project to have more columns

		for (const auto &col_id : input.column_ids) {
			if (IsRowIdColumnId(col_id)) {
				gstate.is_rowid.push_back(true);
			} else {
				gstate.is_rowid.push_back(false);
				gstate.names.emplace_back(bind_data.names[col_id]);
			}
		}

		if (gstate.names.size() < bind_data.names.size()) {
			// If we are auto-detecting, but don't need all columns present in the file,
			// then we don't need to throw an error if we encounter an unseen column
			gstate.transform_options.error_unknown_key = false;
		}
	}

	// Place readers where they belong
	if (bind_data.initial_reader) {
		gstate.json_readers.emplace_back(bind_data.initial_reader.get());
	}
	for (const auto &reader : bind_data.union_readers) {
		gstate.json_readers.emplace_back(reader.get());
	}

	return std::move(result);
}

idx_t JSONGlobalTableFunctionState::MaxThreads() const {
	auto &bind_data = state.bind_data;
	if (bind_data.options.format == JSONFormat::NEWLINE_DELIMITED &&
	    bind_data.options.compression == FileCompressionType::UNCOMPRESSED) {
		return state.system_threads;
	}

	if (!state.json_readers.empty() && state.json_readers[0]->IsOpen()) {
		auto &reader = *state.json_readers[0];
		if (reader.IsParallel()) { // Auto-detected parallel scan
			return state.system_threads;
		}
	}

	// One reader per file
	return bind_data.files.size();
}

JSONLocalTableFunctionState::JSONLocalTableFunctionState(ClientContext &context, JSONScanGlobalState &gstate)
    : state(context, gstate) {
}

unique_ptr<LocalTableFunctionState> JSONLocalTableFunctionState::Init(ExecutionContext &context,
                                                                      TableFunctionInitInput &input,
                                                                      GlobalTableFunctionState *global_state) {
	auto &gstate = global_state->Cast<JSONGlobalTableFunctionState>();
	auto result = make_uniq<JSONLocalTableFunctionState>(context.client, gstate.state);

	// Copy the transform options / date format map because we need to do thread-local stuff
	result->state.date_format_map = gstate.state.bind_data.date_format_map;
	result->state.transform_options = gstate.state.transform_options;
	result->state.transform_options.date_format_map = &result->state.date_format_map;

	return std::move(result);
}

idx_t JSONLocalTableFunctionState::GetBatchIndex() const {
	return state.batch_index;
}

static inline void SkipWhitespace(const char *buffer_ptr, idx_t &buffer_offset, const idx_t &buffer_size) {
	for (; buffer_offset != buffer_size; buffer_offset++) {
		if (!StringUtil::CharacterIsSpace(buffer_ptr[buffer_offset])) {
			break;
		}
	}
}

idx_t JSONScanLocalState::ReadNext(JSONScanGlobalState &gstate) {
	allocator.Reset();

	scan_count = 0;
	if (buffer_offset == buffer_size) {
		if (!ReadNextBuffer(gstate)) {
			return scan_count;
		}
		if (current_buffer_handle->buffer_index != 0 && current_reader->GetFormat() == JSONFormat::NEWLINE_DELIMITED) {
			ReconstructFirstObject(gstate);
			scan_count++;
		}
	}
	ParseNextChunk();

	return scan_count;
}

static inline const char *NextNewline(const char *ptr, idx_t size) {
	return (const char *)memchr(ptr, '\n', size);
}

static inline const char *PreviousNewline(const char *ptr) {
	for (ptr--; true; ptr--) {
		const auto &c = *ptr;
		if (c == '\n') {
			break;
		}
	}
	return ptr;
}

static inline const char *NextJSONDefault(const char *ptr, const idx_t size, const char *const end) {
	idx_t parents = 0;
	while (ptr != end) {
		switch (*ptr++) {
		case '{':
		case '[':
			parents++;
			continue;
		case '}':
		case ']':
			parents--;
			break;
		case '"':
			while (ptr != end) {
				auto string_char = *ptr++;
				if (string_char == '"') {
					break;
				} else if (string_char == '\\') {
					if (ptr != end) {
						ptr++; // Skip the escaped char
					}
				}
			}
			break;
		default:
			continue;
		}

		if (parents == 0) {
			break;
		}
	}

	return ptr;
}

static inline const char *NextJSON(const char *ptr, const idx_t size) {
	D_ASSERT(!StringUtil::CharacterIsSpace(*ptr)); // Should be handled before

	const char *const end = ptr + size;
	switch (*ptr) {
	case '{':
	case '[':
	case '"':
		ptr = NextJSONDefault(ptr, size, end);
		break;
	default:
		// Special case: JSON array containing JSON without clear "parents", i.e., not obj/arr/str
		while (ptr != end) {
			switch (*ptr++) {
			case ',':
			case ']':
				ptr--;
				break;
			default:
				continue;
			}
			break;
		}
	}

	return ptr == end ? nullptr : ptr;
}

static inline void TrimWhitespace(JSONString &line) {
	while (line.size != 0 && StringUtil::CharacterIsSpace(line[0])) {
		line.pointer++;
		line.size--;
	}
	while (line.size != 0 && StringUtil::CharacterIsSpace(line[line.size - 1])) {
		line.size--;
	}
}

void JSONScanLocalState::ParseJSON(char *const json_start, const idx_t json_size, const idx_t remaining) {
	yyjson_doc *doc;
	yyjson_read_err err;
	const auto read_flag = // If we return strings, we cannot parse INSITU
	    bind_data.type == JSONScanType::READ_JSON_OBJECTS ? JSONCommon::READ_FLAG : JSONCommon::READ_INSITU_FLAG;
	doc = JSONCommon::ReadDocumentUnsafe(json_start, remaining, read_flag, allocator.GetYYAlc(), &err);
	if (!bind_data.ignore_errors && err.code != YYJSON_READ_SUCCESS) {
		current_reader->ThrowParseError(current_buffer_handle->buffer_index, lines_or_objects_in_buffer, err);
	}
	lines_or_objects_in_buffer++;
	// TODO if we parse more than json_size, throw error

	if (!doc) {
		values[scan_count] = nullptr;
		return;
	}

	// Set the JSONLine and trim
	units[scan_count] = JSONString(json_start, json_size);
	TrimWhitespace(units[scan_count]);
	values[scan_count] = doc->root;
}

static pair<JSONFormat, JSONRecordType> DetectFormatAndRecordType(const char *const buffer_ptr, const idx_t buffer_size,
                                                                  yyjson_alc *alc) {
	// First we do the easy check whether it's NEWLINE_DELIMITED
	auto line_end = NextNewline(buffer_ptr, buffer_size);
	if (line_end != nullptr) {
		idx_t line_size = line_end - buffer_ptr;

		yyjson_read_err error;
		auto doc = JSONCommon::ReadDocumentUnsafe((char *)buffer_ptr, line_size, JSONCommon::READ_FLAG, alc, &error);
		if (error.code == YYJSON_READ_SUCCESS) {
			if (yyjson_is_arr(doc->root)) {
				// It's just one array, let's actually assume ARRAY, not NEWLINE_DELIMITED
				if (yyjson_arr_size(doc->root) == 0 || yyjson_is_obj(yyjson_arr_get(doc->root, 0))) {
					// Either an empty array (assume records), or an array of objects
					return make_pair(JSONFormat::ARRAY, JSONRecordType::RECORDS);
				} else {
					return make_pair(JSONFormat::ARRAY, JSONRecordType::VALUES);
				}
			} else if (yyjson_is_obj(doc->root)) {
				return make_pair(JSONFormat::NEWLINE_DELIMITED, JSONRecordType::RECORDS);
			} else {
				return make_pair(JSONFormat::NEWLINE_DELIMITED, JSONRecordType::VALUES);
			}
		}
	}

	// Skip whitespace
	idx_t buffer_offset = 0;
	SkipWhitespace(buffer_ptr, buffer_offset, buffer_size);
	auto remaining = buffer_size - buffer_offset;

	// We know it's not NEWLINE_DELIMITED at this point, if there's a '{', we know it's not ARRAY either
	// Also if it's fully whitespace we just return something because we don't know
	if (remaining == 0 || buffer_ptr[buffer_offset] == '{') {
		return make_pair(JSONFormat::UNSTRUCTURED, JSONRecordType::RECORDS);
	}

	// We know it's not RECORDS, if it's not '[', it's not ARRAY either
	if (buffer_ptr[buffer_offset] != '[') {
		return make_pair(JSONFormat::UNSTRUCTURED, JSONRecordType::VALUES);
	}

	// It's definitely an ARRAY, but now we have to figure out if there's more than one top-level array
	yyjson_read_err error;
	auto doc = JSONCommon::ReadDocumentUnsafe((char *)buffer_ptr + buffer_offset, remaining, JSONCommon::READ_FLAG, alc,
	                                          &error);
	if (error.code == YYJSON_READ_SUCCESS) {
		// We successfully read something!
		buffer_offset += yyjson_doc_get_read_size(doc);
		remaining = buffer_size - buffer_offset;

		// Check if there's more than one array
		JSONCommon::ReadDocumentUnsafe((char *)buffer_ptr + buffer_offset, remaining, JSONCommon::READ_FLAG, alc,
		                               &error);
		if (error.code == YYJSON_READ_ERROR_EMPTY_CONTENT) {
			// Just one array, check what's in there
			if (yyjson_is_obj(doc->root)) {
				return make_pair(JSONFormat::ARRAY, JSONRecordType::RECORDS);
			} else {
				return make_pair(JSONFormat::ARRAY, JSONRecordType::VALUES);
			}
		} else {
			// More than one array
			return make_pair(JSONFormat::UNSTRUCTURED, JSONRecordType::VALUES);
		}
	}

	// We weren't able to parse an array, could be broken or an array larger than our buffer size, let's skip over '['
	SkipWhitespace(buffer_ptr, ++buffer_offset, --remaining);
	remaining = buffer_size - buffer_offset;

	// If it's '{' we know there's RECORDS in the ARRAY, else it's VALUES
	if (remaining == 0 || buffer_ptr[buffer_offset] == '{') {
		return make_pair(JSONFormat::ARRAY, JSONRecordType::RECORDS);
	}

	// It's not RECORDS, so it must be VALUES
	return make_pair(JSONFormat::ARRAY, JSONRecordType::VALUES);
}

bool JSONScanLocalState::ReadNextBuffer(JSONScanGlobalState &gstate) {
	AllocatedData buffer;
	if (current_reader) {
		// Keep track of this for accurate errors
		current_reader->SetBufferLineOrObjectCount(current_buffer_handle->buffer_index, lines_or_objects_in_buffer);

		// Try to re-use existing buffer
		if (current_buffer_handle && --current_buffer_handle->readers == 0) {
			buffer = current_reader->RemoveBuffer(current_buffer_handle->buffer_index);
		} else {
			buffer = gstate.allocator.Allocate(gstate.buffer_capacity);
		}

		if (current_reader->GetFormat() != JSONFormat::NEWLINE_DELIMITED && !is_last) {
			memcpy(buffer.get(), reconstruct_buffer.get(), prev_buffer_remainder); // Copy last bit of previous buffer
		} else if (gstate.bind_data.type != JSONScanType::SAMPLE) {
			current_reader->CloseJSONFile(); // Close files that are done if we're not sampling
			current_reader = nullptr;
		}
	} else {
		buffer = gstate.allocator.Allocate(gstate.buffer_capacity);
	}
	buffer_ptr = (const char *)buffer.get();

	idx_t buffer_index;
	while (true) {
		if (current_reader) {
			ReadNextBufferInternal(gstate, buffer_index);
			if (buffer_size == 0) {
				if (is_last && gstate.bind_data.type != JSONScanType::SAMPLE) {
					current_reader->CloseJSONFile();
				}
				if (current_reader->IsParallel()) {
					lock_guard<mutex> guard(gstate.lock);
					if (gstate.file_index < gstate.json_readers.size() &&
					    gstate.json_readers[gstate.file_index].get()) {
						gstate.file_index++; // End parallel scan
					}
				}
				current_reader = nullptr;
			} else {
				break; // We read something!
			}
		}

		// This thread needs a new reader
		lock_guard<mutex> guard(gstate.lock);
		if (gstate.file_index == gstate.json_readers.size()) {
			return false; // No more files left
		}

		// Try the next reader
		current_reader = gstate.json_readers[gstate.file_index].get();
		if (current_reader->IsOpen()) {
			if (!current_reader->IsParallel()) {
				// Can only be open from schema detection
				batch_index = gstate.batch_index++;
				gstate.file_index++;
			}
			continue; // Re-enter the loop to start scanning the assigned file
		}

		// Open the file
		current_reader->OpenJSONFile();
		batch_index = gstate.batch_index++;
		if (current_reader->GetFormat() != JSONFormat::AUTO_DETECT) {
			if (!current_reader->IsParallel()) {
				gstate.file_index++;
			}
			continue;
		}

		// We have to detect the JSON format - hold the gstate lock while we do this
		ReadNextBufferInternal(gstate, buffer_index);
		if (buffer_size == 0) {
			gstate.file_index++; // Empty file, move to the next one
			continue;
		}

		auto format_and_record_type = DetectFormatAndRecordType(buffer_ptr, buffer_size, allocator.GetYYAlc());
		current_reader->SetFormat(format_and_record_type.first);
		if (bind_data.options.record_type == JSONRecordType::AUTO_DETECT) {
			current_reader->SetRecordType(format_and_record_type.second);
		}
		if (current_reader->GetFormat() == JSONFormat::ARRAY) {
			SkipOverArrayStart();
		}

		if (bind_data.options.record_type == JSONRecordType::RECORDS &&
		    current_reader->GetRecordType() != JSONRecordType::RECORDS) {
			throw InvalidInputException("Expected file %s to contain records, got non-record JSON instead",
			                            current_reader->GetFileName());
		}
		if (!current_reader->IsParallel()) {
			gstate.file_index++;
		}
		break;
	}
	D_ASSERT(buffer_size != 0); // We should have read something if we got here

	idx_t readers = 1;
	if (current_reader->IsParallel()) {
		readers = is_last ? 1 : 2;
	}

	// Create an entry and insert it into the map
	auto json_buffer_handle = make_uniq<JSONBufferHandle>(buffer_index, readers, std::move(buffer), buffer_size);
	current_buffer_handle = json_buffer_handle.get();
	current_reader->InsertBuffer(buffer_index, std::move(json_buffer_handle));

	prev_buffer_remainder = 0;
	lines_or_objects_in_buffer = 0;

	// YYJSON needs this
	memset((void *)(buffer_ptr + buffer_size), 0, YYJSON_PADDING_SIZE);

	return true;
}

void JSONScanLocalState::ReadNextBufferInternal(JSONScanGlobalState &gstate, idx_t &buffer_index) {
	if (current_reader->GetFileHandle().CanSeek()) {
		ReadNextBufferSeek(gstate, buffer_index);
	} else {
		ReadNextBufferNoSeek(gstate, buffer_index);
	}

	buffer_offset = 0;
	if (buffer_index == 0 && current_reader->GetFormat() == JSONFormat::ARRAY) {
		SkipOverArrayStart();
	}
}

void JSONScanLocalState::ReadNextBufferSeek(JSONScanGlobalState &gstate, idx_t &buffer_index) {
	auto &file_handle = current_reader->GetFileHandle();

	idx_t request_size = gstate.buffer_capacity - prev_buffer_remainder - YYJSON_PADDING_SIZE;
	idx_t read_position;
	idx_t read_size;

	{
		lock_guard<mutex> reader_guard(current_reader->lock);
		buffer_index = current_reader->GetBufferIndex();

		read_size = file_handle.GetPositionAndSize(read_position, request_size);
		is_last = read_size < request_size;

		if (!gstate.bind_data.ignore_errors && read_size == 0 && prev_buffer_remainder != 0) {
			throw InvalidInputException("Invalid JSON detected at the end of file %s", current_reader->GetFileName());
		}

		if (current_reader->GetFormat() == JSONFormat::NEWLINE_DELIMITED) {
			batch_index = gstate.batch_index++;
		}
	}
	buffer_size = prev_buffer_remainder + read_size;
	if (buffer_size == 0) {
		current_reader->SetBufferLineOrObjectCount(buffer_index, 0);
		return;
	}

	// Now read the file lock-free!
	file_handle.ReadAtPosition(buffer_ptr + prev_buffer_remainder, read_size, read_position,
	                           gstate.bind_data.type == JSONScanType::SAMPLE);
}

void JSONScanLocalState::ReadNextBufferNoSeek(JSONScanGlobalState &gstate, idx_t &buffer_index) {
	idx_t request_size = gstate.buffer_capacity - prev_buffer_remainder - YYJSON_PADDING_SIZE;
	idx_t read_size;
	{
		lock_guard<mutex> reader_guard(current_reader->lock);
		buffer_index = current_reader->GetBufferIndex();

		if (current_reader->IsOpen()) {
			read_size = current_reader->GetFileHandle().Read(buffer_ptr + prev_buffer_remainder, request_size,
			                                                 gstate.bind_data.type == JSONScanType::SAMPLE);
		} else {
			read_size = 0;
		}
		is_last = read_size < request_size;

		if (!gstate.bind_data.ignore_errors && read_size == 0 && prev_buffer_remainder != 0) {
			throw InvalidInputException("Invalid JSON detected at the end of file %s", current_reader->GetFileName());
		}

		if (current_reader->GetFormat() == JSONFormat::NEWLINE_DELIMITED) {
			batch_index = gstate.batch_index++;
		}
	}
	buffer_size = prev_buffer_remainder + read_size;
	if (buffer_size == 0) {
		current_reader->SetBufferLineOrObjectCount(buffer_index, 0);
		return;
	}
}

void JSONScanLocalState::SkipOverArrayStart() {
	// First read of this buffer, check if it's actually an array and skip over the bytes
	SkipWhitespace(buffer_ptr, buffer_offset, buffer_size);
	if (buffer_offset == buffer_size) {
		return; // Empty file
	}
	if (buffer_ptr[buffer_offset] != '[') {
		throw InvalidInputException("Expected top-level JSON array with format='array', but first character is '%c'",
		                            buffer_ptr[buffer_offset]);
	}
	SkipWhitespace(buffer_ptr, ++buffer_offset, buffer_size);
}

void JSONScanLocalState::ReconstructFirstObject(JSONScanGlobalState &gstate) {
	D_ASSERT(current_buffer_handle->buffer_index != 0);
	D_ASSERT(current_reader->GetFormat() == JSONFormat::NEWLINE_DELIMITED);

	// Spinlock until the previous batch index has also read its buffer
	JSONBufferHandle *previous_buffer_handle = nullptr;
	while (!previous_buffer_handle) {
		previous_buffer_handle = current_reader->GetBuffer(current_buffer_handle->buffer_index - 1);
	}

	// First we find the newline in the previous block
	auto prev_buffer_ptr = (const char *)previous_buffer_handle->buffer.get() + previous_buffer_handle->buffer_size;
	auto part1_ptr = PreviousNewline(prev_buffer_ptr);
	auto part1_size = prev_buffer_ptr - part1_ptr;

	// Now copy the data to our reconstruct buffer
	const auto reconstruct_ptr = reconstruct_buffer.get();
	memcpy(reconstruct_ptr, part1_ptr, part1_size);
	// Now find the newline in the current block
	auto line_end = NextNewline(buffer_ptr, buffer_size);
	if (line_end == nullptr) { // TODO I don't think we can ignore this even with ignore_errors ...
		throw InvalidInputException("maximum_object_size of %llu bytes exceeded (>%llu bytes), is the JSON valid?",
		                            bind_data.maximum_object_size, buffer_size - buffer_offset);
	} else {
		line_end++;
	}
	idx_t part2_size = line_end - buffer_ptr;

	idx_t line_size = part1_size + part2_size;
	if (line_size > bind_data.maximum_object_size) {
		throw InvalidInputException("maximum_object_size of %llu bytes exceeded (%llu bytes), is the JSON valid?",
		                            bind_data.maximum_object_size, line_size);
	}

	// And copy the remainder of the line to the reconstruct buffer
	memcpy(reconstruct_ptr + part1_size, buffer_ptr, part2_size);
	memset((void *)(reconstruct_ptr + line_size), 0, YYJSON_PADDING_SIZE);
	buffer_offset += part2_size;

	// We copied the object, so we are no longer reading the previous buffer
	if (--previous_buffer_handle->readers == 0) {
		current_reader->RemoveBuffer(current_buffer_handle->buffer_index - 1);
	}

	ParseJSON((char *)reconstruct_ptr, line_size, line_size);
}

void JSONScanLocalState::ParseNextChunk() {
	auto buffer_offset_before = buffer_offset;

	const auto format = current_reader->GetFormat();
	for (; scan_count < STANDARD_VECTOR_SIZE; scan_count++) {
		auto json_start = buffer_ptr + buffer_offset;
		idx_t remaining = buffer_size - buffer_offset;
		if (remaining == 0) {
			break;
		}
		const char *json_end = format == JSONFormat::NEWLINE_DELIMITED ? NextNewline(json_start, remaining)
		                                                               : NextJSON(json_start, remaining);
		if (json_end == nullptr) {
			// We reached the end of the buffer
			if (!is_last) {
				// Last bit of data belongs to the next batch
				if (format != JSONFormat::NEWLINE_DELIMITED) {
					const auto reconstruct_ptr = reconstruct_buffer.get();
					memcpy(reconstruct_ptr, json_start, remaining);
					prev_buffer_remainder = remaining;
				}
				buffer_offset = buffer_size;
				break;
			}
			json_end = json_start + remaining;
		}

		idx_t json_size = json_end - json_start;
		ParseJSON((char *)json_start, json_size, remaining);
		buffer_offset += json_size;

		if (format == JSONFormat::ARRAY) {
			SkipWhitespace(buffer_ptr, buffer_offset, buffer_size);
			if (buffer_ptr[buffer_offset] == ',' || buffer_ptr[buffer_offset] == ']') {
				buffer_offset++;
			} else { // We can't ignore this error, even with 'ignore_errors'
				yyjson_read_err err;
				err.code = YYJSON_READ_ERROR_UNEXPECTED_CHARACTER;
				err.msg = "unexpected character";
				err.pos = json_size;
				current_reader->ThrowParseError(current_buffer_handle->buffer_index, lines_or_objects_in_buffer, err);
			}
		}
		SkipWhitespace(buffer_ptr, buffer_offset, buffer_size);
	}

	total_read_size += buffer_offset - buffer_offset_before;
	total_tuple_count += scan_count;
}

yyjson_alc *JSONScanLocalState::GetAllocator() {
	return allocator.GetYYAlc();
}

void JSONScanLocalState::ThrowTransformError(idx_t object_index, const string &error_message) {
	D_ASSERT(current_reader);
	D_ASSERT(current_buffer_handle);
	D_ASSERT(object_index != DConstants::INVALID_INDEX);
	auto line_or_object_in_buffer = lines_or_objects_in_buffer - scan_count + object_index;
	current_reader->ThrowTransformError(current_buffer_handle->buffer_index, line_or_object_in_buffer, error_message);
}

} // namespace duckdb
