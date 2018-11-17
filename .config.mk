CFLAGS   += -pthread
CXXFLAGS += -std=c++1z
LDFLAGS  += -lstdc++fs -pthread

xml.deps       = pugixml util
sqlite.deps    = sqlite-amal util smcpp
crypto.deps    = md5
net.deps       = util
conv.deps      = util

# Must be in order of dependencies.
top-level-folders = util conv xml sqlite crypto net test

main.deps = $(top-level-folders)
test.deps = $(filter-out test,$(top-level-folders))

main_is = main
test_is = test

$(call enter_all,$(top-level-folders))

$(call make_exe,main,main)
