#include <darlingserver/prefix-setup.hpp>
#include <filesystem>
#include <fstream>
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string>
namespace fs = std::filesystem;
static void put(const fs::path& p, const char* value) { std::ofstream(p) << value; }
static std::string read(const fs::path& p) { std::ifstream f(p); return std::string(std::istreambuf_iterator<char>(f), {}); }
int main(int argc, char** argv) {
 if(argc != 2) return 1;
 fs::path root(argv[1]), lower=root/"lower", upper=root/"upper", outside=root/"outside";
 fs::create_directories(lower/"Library/Preferences"); fs::create_directories(lower/"Documents"); fs::create_directories(upper/"Library"); fs::create_directories(outside);
 put(lower/"Library/Preferences/default", "new"); put(lower/"Library/existing", "template"); put(upper/"Library/existing", "user"); put(lower/"Documents/foreign", "bad"); put(outside/"sentinel", "safe");
 fs::create_directory_symlink(outside, upper/"Documents"); fs::create_symlink(outside/"sentinel", lower/"source-link");
 int l=open(lower.c_str(), O_RDONLY|O_DIRECTORY), u=open(upper.c_str(), O_RDONLY|O_DIRECTORY);
 bool ok=DarlingServer::PrefixSetup::fillMissingTree(l,u); close(l);close(u);
 if(!ok || read(upper/"Library/Preferences/default")!="new" || read(upper/"Library/existing")!="user" || fs::exists(outside/"foreign") || fs::exists(upper/"source-link") || read(outside/"sentinel")!="safe") return 2;
 fs::create_directories(lower/"new-dir");put(lower/"new-dir/file", "copied");fs::create_directory_symlink(outside, upper/"new-dir");
 timespec old[2]={{1000,0},{1000,0}}; if(utimensat(AT_FDCWD,(upper/"new-dir").c_str(),old,AT_SYMLINK_NOFOLLOW)) return 3;
 l=open(lower.c_str(),O_RDONLY|O_DIRECTORY);u=open(upper.c_str(),O_RDONLY|O_DIRECTORY);ok=DarlingServer::PrefixSetup::copyTree(l,u,false);close(l);close(u);
 if(!ok || fs::is_symlink(upper/"new-dir") || read(upper/"new-dir/file")!="copied" || fs::exists(outside/"file") || read(outside/"sentinel")!="safe") return 4;
 return 0;
}
