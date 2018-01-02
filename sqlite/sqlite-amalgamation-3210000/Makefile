ifndef root
    include $(dir $(lastword $(MAKEFILE_LIST)))../Makefile
else
    # Must enter in order of dependencies.
    locations := # Put any additional sublocations here
    $(call enter_all,$(locations))

    # We  want to specify this flag so that sqlite doesn't try to
    # invoke libdl functions to load  extensions  which  then  re-
    # quires linking against libdl, which we don't need.
    sqlite-amal.cflags += -DSQLITE_OMIT_LOAD_EXTENSION
    # Despite how it sounds, the following flag actually _allows_
    # implicit  fallthrough  by  disabling gcc's warning about im-
    # plicit  fallthroughs. This may not be a good flag to use in
    # general since we  do  want  to  catch implicit fallthroughs,
    # but in this library (3rd party code) which we know has been
    # tested and which we don't want to change, the easiest  solu-
    # tion is to just disable the warning.
    sqlite-amal.cflags += -Wno-implicit-fallthrough

    $(call make_ar,sqlite-amal,sqlite-amal)
endif
