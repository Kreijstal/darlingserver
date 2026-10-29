// Native loader overrides obey the same privilege and path policy as libexec.
#include <darlingserver/run-mode.hpp>
#include <darlingserver/config.hpp>
#include <string>
using namespace DarlingServer;
int main() {
    using RunMode::Mode;
    const std::string system = "/opt/darling";
    const std::string normal = system + "/usr/libexec/darling/mldr";
    std::string path, error;
    for (auto mode : {Mode::Root, Mode::NonRoot, Mode::Rootless}) {
        if (!RunMode::resolveMldrPath(mode, system, nullptr, path, error) || path != normal) return 1;
        if (!RunMode::resolveMldrPath(mode, system, "", path, error) || path != normal) return 2;
        if (!RunMode::resolveMldrPath(mode, system, "/data/app/native/libmldr.so", path, error)) return 3;
        if (path != (mode == Mode::Root ? normal : "/data/app/native/libmldr.so")) return 4;
        for (const auto* bad : {"relative", "/a/../b", "/a/./b", "/a\nb", "/a\x7f"}) {
            bool ok = RunMode::resolveMldrPath(mode, system, bad, path, error);
            if ((mode == Mode::Root) != ok) return 5;
            if (mode == Mode::Root && path != normal) return 6;
        }
    }
    if (RunMode::resolveMldrPath(Mode::NonRoot, system, ("/" + std::string(5000, 'x')).c_str(), path, error)) return 7;
    Config::setLibexecPath(system);
    Config::setMldrPath("/data/app/native/libmldr.so");
    if (Config::defaultMldrPath() != "/data/app/native/libmldr.so") return 8;
    return 0;
}
