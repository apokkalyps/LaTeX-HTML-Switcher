#include <gtest/gtest.h>
#include "pch.h"
#include "../C++/Toolbox.hpp"
#include "../C++/Toolbox.cpp"
#include "../C++/File.hpp"
#include "../C++/File.cpp"

#ifdef _WIN32
#include <direct.h>
#endif

// ////////////////////////////////////////////////////////////////////////////////////////////// //
//                             ALL TESTS FOR THE File MODULE                                      //
// ////////////////////////////////////////////////////////////////////////////////////////////// //

// All informations for all files being used.
static file_info_data fid_middlesize(287815, FNAME_MIDDLESIZE, 'l', '\xEF', true, File::ENC_DEFAULT);
static file_info_data fid_unexisting(0, FNAME_UNEXISTING, 0, 0, false, File::ENC_UNKNOWN);
static file_info_data fid_smallfile_utf16le(38, FNAME_SMALL_UTF16LE, '\xFF', '\x00', true, File::ENC_UTF16LE, 2);

static constexpr int LEN_WORKDIR = 511;
static char buffer_workdir[LEN_WORKDIR];

// Motherclass for File:: test cases.
class TestFile : public ::testing::Test {
protected:
	TestFile(const file_info_data& fid) :
		fid(fid)
	{}

	void CheckSize()
	{
		File::filesize_t size = File::Size(fid.name);
		if (fid.size)
			ASSERT_NE(size, 0);
		EXPECT_EQ(size, fid.size);
	}

	void CheckExists()
	{
		bool result = File::Exists(fid.name);
		if (fid.exists)
			EXPECT_TRUE(result);
		else
			EXPECT_FALSE(result);
	}

	void CheckEncoding()
	{
		EXPECT_EQ(File::Encoding(fid.name), fid.encoding);
	}

	void CheckOpen()
	{
		std::ifstream ifs;
		File::Open(ifs, fid.name);
		if (fid.exists)
		{
			ASSERT_TRUE(ifs.good());
			EXPECT_EQ(ifs.tellg(), fid.offset);
			ifs.seekg(0, std::ios_base::beg);
			EXPECT_TRUE(ifs.good());
			EXPECT_EQ(static_cast<char>(ifs.peek()), fid.firstByte);
			ifs.seekg(-1, std::ios_base::end);
			EXPECT_EQ(static_cast<char>(ifs.peek()), fid.lastByte);
		}
		else
			ASSERT_FALSE(ifs.good());
	}

	void SetUp() override
	{
		if (fid.exists)
			ASSERT_TRUE(File::Exists(fid.name));
		else
			if (File::Exists(fid.name))
				File::Delete(fid.name, true);
	}

	void TearDown() override
	{
		if (fid.exists)
			ASSERT_TRUE(File::Exists(fid.name));
		else
			ASSERT_FALSE(File::Exists(fid.name));
	}

	file_info_data fid;
};

#define CLASS_TEST_FILE(classname, fidname) \
class classname : public TestFile {\
protected: \
	classname () : TestFile( fidname ) {} \
}

CLASS_TEST_FILE(TestUnexistingFile, fid_unexisting);
CLASS_TEST_FILE(TestMiddleWeightFile, fid_middlesize);
CLASS_TEST_FILE(TestSmallFileUTF16LE, fid_smallfile_utf16le);

// Test fixtures
#if 1
TEST_F(TestMiddleWeightFile, VerifySize) {
	CheckSize();
}

TEST_F(TestMiddleWeightFile, VerifyExists) {
	CheckExists();
}

TEST_F(TestMiddleWeightFile, VerifyEncoding) {
	CheckEncoding();
}

TEST_F(TestMiddleWeightFile, VerifyOpen) {
	CheckOpen();
}

TEST_F(TestUnexistingFile, VerifySize) {
	CheckSize();
}

TEST_F(TestUnexistingFile, VerifyExists) {
	CheckExists();
}

TEST_F(TestUnexistingFile, VerifyEncoding) {
	CheckEncoding();
}

TEST_F(TestUnexistingFile, VerifyOpen) {
	CheckOpen();
}

TEST_F(TestSmallFileUTF16LE, VerifySize) {
	CheckSize();
}

TEST_F(TestSmallFileUTF16LE, VerifyExists) {
	CheckExists();
}

TEST_F(TestSmallFileUTF16LE, VerifyEncoding) {
	CheckEncoding();
}

TEST_F(TestSmallFileUTF16LE, VerifyOpen) {
	CheckOpen();
}

TEST(TestIsDir, ActualFolder)
{	
	ASSERT_TRUE(File::IsDir(FNAME_PREFIX))
#ifdef _WIN32
		<< _getcwd(buffer_workdir, LEN_WORKDIR)
#endif
		; 
}

TEST(TestIsDir, IsAFile)
{	ASSERT_FALSE(File::IsDir(FNAME_SMALL_UTF16LE));}

TEST(TestIsDir, Unexisting)
{	ASSERT_FALSE(File::IsDir(FNAME_UNEXISTING)); }

TEST(TestIsDir, NewFolder) {
	const File::filename_t& filename = FNAME_TEMP;
	ASSERT_TRUE(File::CreateFolder(filename));
	EXPECT_TRUE(File::IsDir(filename));
	ASSERT_TRUE(File::Delete(filename, false));
}

TEST(TestDelete, Unexisting)
{	ASSERT_FALSE(File::Delete(FNAME_UNEXISTING)); }

#endif

// Main function
int main(int argc, char** argv)
{
#ifdef I_Want_Mem_Leaks
	_CrtMemState states[3];
	_CrtMemCheckpoint(&states[0]);
#endif

	::testing::InitGoogleTest(&argc, argv);
	auto res = RUN_ALL_TESTS();

#ifdef I_Want_Mem_Leaks
	_CrtMemCheckpoint(&states[1]);
	if (_CrtMemDifference(&states[2], &states[0], &states[1]))
		_CrtMemDumpStatistics(&states[2]);
#endif
	return res;
}
