CXXFLAGS += -std=c++1z

LDFLAGS += -lstdc++fs

main.deps      = xml-utils project
project.deps   = xml-utils
xml-utils.deps = pugixml util
test.deps      = util

main_is = main
test_is = test

$(call enter_all,util xml-utils project test)
$(call make_exe,main,main$(opt-suffix))
