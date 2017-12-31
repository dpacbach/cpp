ifndef root
    include $(dir $(lastword $(MAKEFILE_LIST)))../Makefile
else
    # Must enter in order of dependencies.
    locations := # Put any additional sublocations here
    $(call enter_all,$(locations))

    TP_LINK_test := -lsqlite3
    $(call make_exe,test,test)
endif
