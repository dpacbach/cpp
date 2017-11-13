main.deps = pugixml

main_is = main

$(call enter_all,pugixml)
$(call make_exe,main,xml-test)
