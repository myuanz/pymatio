% MATLAB script to generate various .mat files for testing Python mat file reading library
% Enable diary to log execution
script_dir = fileparts(mfilename('fullpath'));
diary(fullfile(script_dir, 'matlab_execution.log'));
diary on;

% Supports different formats (mat5, mat7.3) and covers various data types
% Generated files will be saved in the data/ directory relative to this script

% Create data directory if it doesn't exist
data_dir = fullfile(script_dir, 'data');
if ~exist(data_dir, 'dir')
    mkdir(data_dir);
end

% Define formats to generate
formats = {'mat5', 'mat7.3'};

% Generate scalar values
fprintf('Generating scalar values...\n');
double_scalar = 3.141592653589793;
float_scalar = single(2.71828);
int32_scalar = int32(42);
uint32_scalar = uint32(100);
int64_scalar = int64(-9223372036854775808);
uint64_scalar = uint64(18446744073709551615);
logical_scalar = true;
char_scalar = 'A';

for format = formats
    if strcmp(format{1}, 'mat5')
        save(fullfile(data_dir, 'test_scalars_mat5.mat'), ...
             'double_scalar', 'float_scalar', 'int32_scalar', 'uint32_scalar', ...
             'int64_scalar', 'uint64_scalar', 'logical_scalar', 'char_scalar', '-v7');
    else
        save(fullfile(data_dir, 'test_scalars_mat73.mat'), ...
             'double_scalar', 'float_scalar', 'int32_scalar', 'uint32_scalar', ...
             'int64_scalar', 'uint64_scalar', 'logical_scalar', 'char_scalar', '-v7.3');
    end
end

% Generate vector values
fprintf('Generating vector values...\n');
double_vector = linspace(1, 100, 10);
float_vector = single(rand(1, 10));
int32_vector = int32(1:10);
uint32_vector = uint32(0:9);
int64_vector = int64(-5:4);
uint64_vector = uint64(100:109);
logical_vector = logical([1 0 1 0 1 0 1 0 1 0]);
char_vector = 'Hello World';

for format = formats
    if strcmp(format{1}, 'mat5')
        save(fullfile(data_dir, 'test_vectors_mat5.mat'), ...
             'double_vector', 'float_vector', 'int32_vector', 'uint32_vector', ...
             'int64_vector', 'uint64_vector', 'logical_vector', 'char_vector', '-v7');
    else
        save(fullfile(data_dir, 'test_vectors_mat73.mat'), ...
             'double_vector', 'float_vector', 'int32_vector', 'uint32_vector', ...
             'int64_vector', 'uint64_vector', 'logical_vector', 'char_vector', '-v7.3');
    end
end

% Generate matrix values
fprintf('Generating matrix values...\n');
double_matrix = magic(5);
float_matrix = single(rand(5, 5));
int32_matrix = int32(hilb(5));
uint32_matrix = uint32(vander(1:5));
int64_matrix = int64(ones(5, 5) * -1);
uint64_matrix = uint64(eye(5));
logical_matrix = logical(rand(5, 5) > 0.5);
char_matrix = ['Hello'; 'World'; 'Test!'];

for format = formats
    if strcmp(format{1}, 'mat5')
        save(fullfile(data_dir, 'test_matrices_mat5.mat'), ...
             'double_matrix', 'float_matrix', 'int32_matrix', 'uint32_matrix', ...
             'int64_matrix', 'uint64_matrix', 'logical_matrix', 'char_matrix', '-v7');
    else
        save(fullfile(data_dir, 'test_matrices_mat73.mat'), ...
             'double_matrix', 'float_matrix', 'int32_matrix', 'uint32_matrix', ...
             'int64_matrix', 'uint64_matrix', 'logical_matrix', 'char_matrix', '-v7.3');
    end
end

% Generate multidimensional arrays
fprintf('Generating multidimensional arrays...\n');
double_3d = rand(3, 3, 3);
float_3d = single(rand(3, 3, 3));
int32_3d = int32(rand(3, 3, 3) * 100);
double_4d = rand(2, 2, 2, 2);

for format = formats
    if strcmp(format{1}, 'mat5')
        save(fullfile(data_dir, 'test_multidim_mat5.mat'), ...
             'double_3d', 'float_3d', 'int32_3d', 'double_4d', '-v7');
    else
        save(fullfile(data_dir, 'test_multidim_mat73.mat'), ...
             'double_3d', 'float_3d', 'int32_3d', 'double_4d', '-v7.3');
    end
end

% Generate cell arrays
fprintf('Generating cell arrays...\n');
simple_cell = {1, 'hello', [1 2 3], true};
nested_cell = {simple_cell, {1:10}, 'nested'};
empty_cell = {};
cell_of_cells = {{1, 2}, {3, 4}, {5, 6}};

for format = formats
    if strcmp(format{1}, 'mat5')
        save(fullfile(data_dir, 'test_cells_mat5.mat'), ...
             'simple_cell', 'nested_cell', 'empty_cell', 'cell_of_cells', '-v7');
    else
        save(fullfile(data_dir, 'test_cells_mat73.mat'), ...
             'simple_cell', 'nested_cell', 'empty_cell', 'cell_of_cells', '-v7.3');
    end
end

% Generate structures
fprintf('Generating structures...\n');
simple_struct.name = 'Test';
simple_struct.value = 42;
simple_struct.data = [1 2 3 4 5];

nested_struct.level1.field1 = 'value1';
nested_struct.level1.field2 = 123;
nested_struct.level2.field1 = [1 2 3];
nested_struct.level2.field2 = {'a', 'b', 'c'};

struct_array(1).name = 'Array1';
struct_array(1).value = 10;
struct_array(2).name = 'Array2';
struct_array(2).value = 20;
struct_array(3).name = 'Array3';
struct_array(3).value = 30;

for format = formats
    if strcmp(format{1}, 'mat5')
        save(fullfile(data_dir, 'test_structs_mat5.mat'), ...
             'simple_struct', 'nested_struct', 'struct_array', '-v7');
    else
        save(fullfile(data_dir, 'test_structs_mat73.mat'), ...
             'simple_struct', 'nested_struct', 'struct_array', '-v7.3');
    end
end

% Generate string arrays (MATLAB R2016b+)
fprintf('Generating string arrays...\n');
string_array = string({'Hello', 'World', 'MATLAB', 'Python'});
nested_string_array = {string({'a', 'b'}), string({'c', 'd'})};
empty_string_array = string.empty;

for format = formats
    if strcmp(format{1}, 'mat5')
        save(fullfile(data_dir, 'test_strings_mat5.mat'), ...
             'string_array', 'nested_string_array', 'empty_string_array', '-v7');
    else
        save(fullfile(data_dir, 'test_strings_mat73.mat'), ...
             'string_array', 'nested_string_array', 'empty_string_array', '-v7.3');
    end
end

% Generate complex numbers
fprintf('Generating complex numbers...\n');
complex_scalar = 3 + 4i;
complex_vector = [1+2i, 3+4i, 5+6i, 7+8i, 9+10i];
complex_matrix = rand(3, 3) + rand(3, 3) * 1i;
complex_float = single(2 + 3i);

for format = formats
    if strcmp(format{1}, 'mat5')
        save(fullfile(data_dir, 'test_complex_mat5.mat'), ...
             'complex_scalar', 'complex_vector', 'complex_matrix', 'complex_float', '-v7');
    else
        save(fullfile(data_dir, 'test_complex_mat73.mat'), ...
             'complex_scalar', 'complex_vector', 'complex_matrix', 'complex_float', '-v7.3');
    end
end

% Generate enumerations
fprintf('Generating enumerations...\n');
color_enum = Color.Red;
color_array = [Color.Green, Color.Blue, Color.Red];

for format = formats
    if strcmp(format{1}, 'mat5')
        save(fullfile(data_dir, 'test_enums_mat5.mat'), ...
             'color_enum', 'color_array', '-v7');
    else
        save(fullfile(data_dir, 'test_enums_mat73.mat'), ...
             'color_enum', 'color_array', '-v7.3');
    end
end

% Generate time series data
fprintf('Generating time series data...\n');
time = datetime('now') - hours(24):minutes(30):datetime('now');
data = rand(size(time));
time_num = datenum(time);
time_series = timeseries(data, time_num);

for format = formats
    if strcmp(format{1}, 'mat5')
        save(fullfile(data_dir, 'test_timeseries_mat5.mat'), ...
             'time', 'data', 'time_series', '-v7');
    else
        save(fullfile(data_dir, 'test_timeseries_mat73.mat'), ...
             'time', 'data', 'time_series', '-v7.3');
    end
end

% Generate empty arrays
fprintf('Generating empty arrays...\n');
empty_double = [];
empty_int32 = int32.empty;
empty_logical = logical.empty;
empty_char = '';
empty_cell = {};
empty_struct = struct();

for format = formats
    if strcmp(format{1}, 'mat5')
        save(fullfile(data_dir, 'test_empty_mat5.mat'), ...
             'empty_double', 'empty_int32', 'empty_logical', 'empty_char', ...
             'empty_cell', 'empty_struct', '-v7');
    else
        save(fullfile(data_dir, 'test_empty_mat73.mat'), ...
             'empty_double', 'empty_int32', 'empty_logical', 'empty_char', ...
             'empty_cell', 'empty_struct', '-v7.3');
    end
end

% Generate mixed data types
fprintf('Generating mixed data types...\n');
mixed_data.name = 'Mixed Test';
mixed_data.scalars = {1, 2.5, true, 'x'};
mixed_data.vector = 1:10;
mixed_data.matrix = rand(3, 3);
mixed_data.cell = {1, 'hello', [1 2 3]};
mixed_data.struct = struct('a', 1, 'b', 'test');

for format = formats
    if strcmp(format{1}, 'mat5')
        save(fullfile(data_dir, 'test_mixed_mat5.mat'), 'mixed_data', '-v7');
    else
        save(fullfile(data_dir, 'test_mixed_mat73.mat'), 'mixed_data', '-v7.3');
    end
end

fprintf('All .mat files generated successfully!\n');
fprintf('Saved to: %s\n', data_dir);
% List the generated files
fprintf('Generated files:\n');
files = dir(fullfile(data_dir, '*.mat'));
for i = 1:length(files)
    fprintf('  %s\n', files(i).name);
end
diary off;