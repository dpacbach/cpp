CXXFLAGS += -std=c++1z

LDFLAGS += -lstdc++fs

main.deps      = xml-utils project
project.deps   = xml-utils
xml-utils.deps = pugixml util

main_is = main

top-level-folders = util xml-utils project

ifeq (undefined,$(origin OPT))
    top-level-folders += test
    test.deps          = util
    test_is            = test
endif

$(call enter_all,$(top-level-folders))
$(call make_exe,main,main)
