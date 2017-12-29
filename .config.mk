CXXFLAGS += -std=c++1z -pthread

LDFLAGS += -lstdc++fs -pthread

project.deps   = xml-utils
xml-utils.deps = pugixml util
sqlite.deps    = util

top-level-folders = util xml-utils sqlite project

main.deps = $(top-level-folders)

ifeq (undefined,$(origin OPT))
    top-level-folders += test
    test.deps          = $(filter-out test,$(top-level-folders))
    test_is            = test
endif

main_is = main

$(call enter_all,$(top-level-folders))
$(call make_exe,main,main)
