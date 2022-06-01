#include <NovelTea/Zip/Zip.hpp>
#include <NovelTea/Zip/zip.h>
#include <stdexcept>
#include <iostream>

namespace NovelTea
{

ZipReader::ZipReader(const std::string &fileName)
{
	if (!(m_zip = zip_open(fileName.c_str(), 0, 'r')))
		throw std::runtime_error("Failed to open file as zip archive.");
}

ZipReader::ZipReader(const char *data, size_t size)
{
	if (!(m_zip = zip_stream_open(data, size, 0, 'r')))
		throw std::runtime_error("Data could not be read as zip archive.");
}

ZipReader::~ZipReader()
{
	zip_close(m_zip);
}

std::string ZipReader::read(const std::string &fileName)
{
	std::string result;
	if (zip_entry_open(m_zip, fileName.c_str()) == 0)
	{
		result.resize(zip_entry_size(m_zip));
		zip_entry_noallocread(m_zip, &result[0], result.size());
		zip_entry_close(m_zip);
	}
	return result;
}

std::vector<std::string> ZipReader::getFileList(const std::string &prefix)
{
	std::vector<std::string> fileNames;

	int n = zip_entries_total(m_zip);
	for (int i = 0; i < n; ++i) {
		if (zip_entry_openbyindex(m_zip, i) == 0)
		{
			if (!zip_entry_isdir(m_zip)) {
				std::string fileName = zip_entry_name(m_zip);
				if (prefix.empty() || fileName.find(prefix) == 0)
					fileNames.push_back(fileName);
			}
			zip_entry_close(m_zip);
		}
	}

	return fileNames;
}

ZipWriter::ZipWriter(const std::string &fileName)
{
	if (!(m_zip = zip_open(fileName.c_str(), ZIP_DEFAULT_COMPRESSION_LEVEL, 'w')))
//	if (!(m_zip = zip_open(fileName.c_str(), 9, 'w')))
		throw std::runtime_error("Failed to write zip archive.");
}

ZipWriter::~ZipWriter()
{
	zip_close(m_zip);
}

void ZipWriter::write(const std::string &fileName, const std::string &contents)
{
	if (zip_entry_open(m_zip, fileName.c_str()) == 0)
	{
		zip_entry_write(m_zip, contents.data(), contents.size());
		zip_entry_close(m_zip);
	}
	else
		throw std::runtime_error("Failed to write zip archive entry: " + fileName);
}

} // namespace NovelTea
