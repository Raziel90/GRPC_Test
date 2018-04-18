#include <iostream>
#include <fstream>
#include <memory>
#include <string>
#include <vector> 




int main(int argc, char const *argv[])
{
	int buffer_size = 1024;
	if( argc < 3) 
	{
		std::cerr<< "Syntax: file_copy <in_file> <out_file>"<<std::endl;
		return -1;
	}
	std::string in_filename(argv[1]);
	std::string out_filename(argv[2]);

	std::ifstream in_file(in_filename,std::ifstream::binary);
	std::ofstream out_file(out_filename,std::ofstream::binary|std::ofstream::trunc);

	in_file.seekg (0, in_file.end);
	int file_length = in_file.tellg();
	in_file.seekg (0, in_file.beg);

	int loaded_byte = 0;
	int byte_to_read = std::min(buffer_size,file_length-loaded_byte);
	std::vector<char> buffer(buffer_size + 1,0);

	while(in_file.read(buffer.data(),byte_to_read)&&byte_to_read)
	{	
		out_file.write(buffer.data(),byte_to_read);
		loaded_byte += byte_to_read;
		byte_to_read = std::min(buffer_size,file_length-loaded_byte);
	}
	
	in_file.close();
	out_file.close();


	return 0;
}