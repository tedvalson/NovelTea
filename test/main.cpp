#include <gtest/gtest.h>
#include <NovelTea/AssetManager.hpp>

std::string g_dir;

int main(int argc, char **argv)
{
	// Globally define bin path for tests involving external files
	::testing::internal::FilePath fp(argv[0]);
	g_dir = fp.RemoveFileName().string();
	
	NovelTea::AssetPath::set(g_dir + "../../res/assets/");
	
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}
