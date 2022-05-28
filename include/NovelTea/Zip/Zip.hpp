#ifndef NOVELTEA_ZIP_HPP
#define NOVELTEA_ZIP_HPP

#include <vector>
#include <string>

struct zip_t;

namespace NovelTea
{

class ZipReader
{
public:
	ZipReader(const std::string &fileName = std::string());
	ZipReader(const char *stream, size_t size);
	virtual ~ZipReader();

	std::string read(const std::string &fileName);
	void write(const std::string &fileName, const std::string &contents);

	std::vector<std::string> getFileList(const std::string &prefix = std::string());

private:
	zip_t* m_zip;
};

class ZipWriter
{
public:
	ZipWriter(const std::string &fileName = std::string());
	virtual ~ZipWriter();
	void write(const std::string &fileName, const std::string &contents);
private:
	zip_t* m_zip;
};

} // namespace NovelTea

#endif // NOVELTEA_ZIP_HPP
