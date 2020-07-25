#include <QString>
#include <QStringList>
#include "sigil_constants.h"

#if _WIN32
#include <QProcessEnvironment>

// Windows barks about getenv or _wgetenv. This elicits no warnings and works with unicode paths
const QString SIGIL_PREFS_DIR = QProcessEnvironment::systemEnvironment().value("SIGIL_PREFS_DIR", "").trimmed();

const QString PATH_LIST_DELIM = ";";
const QString PYTHON_MAIN_PATH = "";  // Bundled Python interpreter is in same dir as Sigil exe now
const QStringList PYTHON_SYS_PATHS = QStringList() << "/Lib" << "/DLLs" << "/Lib/site-packages";
#else
const QString SIGIL_PREFS_DIR = QString(getenv("SIGIL_PREFS_DIR"));
#endif

#if __APPLE__
const QString PATH_LIST_DELIM = ":";
const QString PYTHON_MAIN_PREFIX = "/Frameworks/Python.framework/Versions/3.8";
const QString PYTHON_MAIN_BIN_PATH = PYTHON_MAIN_PREFIX + "/bin/python3";
const QString PYTHON_LIB_PATH = "/lib/python3.8";
const QString PYTHON_SITE_PACKAGES = PYTHON_MAIN_PREFIX + PYTHON_LIB_PATH + "/site-packages";
const QStringList PYTHON_SYS_PATHS = QStringList () << "/lib-dynload" << "/site-packages";
#endif

#if !defined(_WIN32) && !defined(__APPLE__)
const QString PATH_LIST_DELIM = ":";
// Runtime env var override of Sigil's 'share/sigil' directory
const QString sigil_extra_root = QString(getenv("SIGIL_EXTRA_ROOT"));
// Runtime env var override of hunspell dictionaries directory to use
const QString hunspell_dicts_override = QString(getenv("SIGIL_DICTIONARIES"));
// Runtime env var to force the use of Sigil's darkmode palette instead of platform QPA themes/styles
const QString force_sigil_darkmode_palette = QString(getenv("FORCE_SIGIL_DARKMODE_PALETTE"));
// Standard build-time location of Sigil's 'share/sigil' directory. Set in src/CMakeLists.txt with the line:
// set_source_files_properties( sigil_constants.cpp PROPERTIES COMPILE_DEFINITIONS SIGIL_SHARE_ROOT="${SIGIL_SHARE_ROOT}" )
const QString sigil_share_root = QString(SIGIL_SHARE_ROOT);
const bool dicts_are_bundled = DICTS_ARE_BUNDLED;
const QString extra_dict_dirs = QString(EXTRA_DICT_DIRS);
const QString mathjax_dir = QString(MATHJAX_DIR);
const QString PYTHON_MAIN_PATH = "/python3/lib/python3.5";
#if __x86_64__ || __ppc64__
const QStringList PYTHON_SYS_PATHS = QStringList () << "/plat-x86_64-linux-gnu" << "/plat-linux" << "/lib-dynload" << "/site-packages";
#else
const QStringList PYTHON_SYS_PATHS = QStringList () << "/plat-i386-linux-gnu" << "/plat-linux" << "/lib-dynload" << "/site-packages";
#endif
#endif
