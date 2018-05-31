from os import environ
from os.path import basename, splitext, relpath, dirname
from subprocess import Popen, PIPE

LOG_FILE = "%s/ycm_log.txt" % environ['HOME']

def make( filename ):
    rel_file = relpath( filename )
    rel_dir  = dirname( relpath( filename ) )
    rel_dir = (rel_dir+'/') if rel_dir else ''
    stem = basename( splitext( rel_file )[0] )
    target = '%s.lib-linux64/%s.o' % (rel_dir, stem)
    # -n means don't actually run the target, just print the commands,
    # -B means act as if the target were out of date even if it is not
    p = Popen( ['/usr/bin/make', 'CLANG=', '-nB', 'V=', target], stdout=PIPE, stderr=PIPE )
    (stdout, stderr) = p.communicate()
    assert p.returncode == 0, 'return code non-zero:\n%s' % stderr
    lines = [l for l in stdout.split( '\n' ) if target in l]
    assert len( lines ) == 1, 'length == %d' % len( lines )
    return lines[0]

def FlagsForFile( filename, **kwargs ):
    with file( LOG_FILE, 'a' ) as f:
        f.write( "-\nfilename: %s\n" % filename )
        flags = make( filename )
        f.write( "cmd:      %s\n" % flags )
    return { 'flags': flags.split() }
