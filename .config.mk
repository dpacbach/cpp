CFLAGS   += -pthread
CXXFLAGS += -std=c++1z
LDFLAGS  += -lstdc++fs -pthread

# Unfortunately, there seems to be an issue where clang is unable
# to compile libstdc++'s std::get applied to a std::variant. This
# seems to be a known issue  and a proper fix apparently involves
# fixing clang. However, so far the only workaround/fix that  has
# been put in place  is  one  on  the  libstdc++  side (i.e., not
# clang) so therefore updating to  a later clang version probably
# will  not help at the moment. Instead, we'd have to point clang
# to  the  latest  version of libstd++, or use libc++. Since this
# would be a pain and since the lack of  std::get  with  variants
# practically  renders them useless, we just set this flag to dis-
# able those parts of the variant code that rely on std::get when
# compiling  with  clang  until  this  issue  is resolved somehow.
#
# See here for updates:
#
# https://bugs.llvm.org/show_bug.cgi?id=33222
# https://bugs.llvm.org/show_bug.cgi?id=31852
ifneq ($(origin CLANG),undefined)
    CXXFLAGS += -DCLANG_LIBSTDCPP_NO_VARIANT
endif

xml.deps       = pugixml util
sqlite.deps    = sqlite-amal util smcpp
crypto.deps    = md5
svn.deps       = xml util
net.deps       = util
conv.deps      = util

# Must be in order of dependencies.
top-level-folders = util conv xml sqlite crypto svn net test

main.deps = $(top-level-folders)
test.deps = $(filter-out test,$(top-level-folders))

main_is = main
test_is = test

$(call enter_all,$(top-level-folders))

$(call make_exe,main,main)
