from os.path import basename, splitext
from subprocess import Popen, PIPE

LOG_FILE = '/home/dsicilia/ycm_log.txt'

def make( filename ):
    stem = basename( splitext( filename )[0] )
    target = '.lib-linux64/%s.o' % stem
    # -n means don't actually run the target, just print the commands,
    # -B means act as if the target were out of date even if it is not
    p = Popen( ['/usr/bin/make', 'CLANG=', '-nB', 'V=', target], stdout=PIPE )
    (stdout, stderr) = p.communicate()
    assert p.returncode == 0, 'return code non-zero'
    lines = [l for l in stdout.split( '\n' ) if target in l]
    assert len( lines ) == 1, 'length == %d' % len( lines )
    return lines[0]

def FlagsForFile( filename, **kwargs ):
    with file( LOG_FILE, 'a' ) as f:
        f.write( "-\nfilename: %s\n" % filename )
        flags = make( filename )
        f.write( "cmd:      %s\n" % flags )
    return { 'flags': flags.split() }
