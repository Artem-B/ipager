#!/bin/env python
from sys import exit;
import os;

SConsignFile()

###############################################
# options
ipager_optfile = [ 'scons.opts', 'user.opts' ]
ipager_options = Options(ipager_optfile)
ipager_options.AddOptions(
    BoolOption('debug', 'build debug version', 0),
    BoolOption('debug_events', 'debug xserve events', 0),

    BoolOption('xinerama', 'support xinerama', 0),

    PathOption('PREFIX', 'install-path base', '/usr/local'),
    PathOption('DESTDIR', 'install to $DESTDIR/$PREFIX', '/')
)


###############################################
# installpaths
ipager_instdir = os.path.join('$DESTDIR','$PREFIX')
ipager_instdir_bin = os.path.join(ipager_instdir,'bin')



###############################################
# environment
ipager_env = Environment(options = ipager_options, ENV = os.environ)

ipager_env.Append(
        CPPFLAGS = [ '-Wall' ],
        CPPPATH = [ '/usr/X11R6/include' ],
        LIBPATH = ['/usr/X11R6/lib']
	)

ipager_options.Update(ipager_env)


#
if ipager_env['debug']:
    ipager_env.AppendUnique(
            CCFLAGS = ['-g', '-ggdb']
	    )

if ipager_env['debug_events']:
    ipager_env.AppendUnique(
            CCFLAGS = ['-g'],
            CPPDEFINES = ['EVENTS_DEBUG'])

ipager_env.AppendUnique(
        LIBS = ['m', 'pthread'],
        CCFLAGS = ['-pthread'])
	
	
	
	

#################################################
# check libs
conf = ipager_env.Configure()


#
# Headers
#

# sys/time.h
if conf.CheckCHeader('sys/time.h'):
  ipager_env.AppendUnique( CPPDEFINES = ['-DHAS_SYS_TIME_H'])

# sys/stat.h
if not conf.CheckCHeader('sys/stat.h'):
    print "missing 'sys/stat.h', install it."
    exit(1)

# sys/types.h
if not conf.CheckCHeader('sys/types.h'):
    print "missing 'sys/types.h', install it."
    exit(1)


#
# Libs
#
# x11
if conf.CheckLibWithHeader('X11', 'X11/Xlib.h', 'C', 'XOpenDisplay(0);', 1):
    ipager_env.AppendUnique(
        LIBS = ['X11','Xmu','Xext'])
else:
    print "missing x11-dev-stuff, install it."
    exit(1)


# xinerama
if conf.env['xinerama'] and conf.CheckLib('Xinerama', 'XineramaQueryScreens', 1):
    ipager_env.AppendUnique(
        CPPDEFINES = ['HAVE_XINERAMA'],
        LIBS = ['Xinerama'])


# imlib2
print "Checking for Imlib2... ",
if not conf.env.WhereIs('imlib2-config'):
    print "cant find 'imlib2-config."
    exit(1)
else:
    imlib2_env = Environment()
    imlib2_env.ParseConfig('imlib2-config --cflags --libs')
    if not imlib2_env.Dictionary()['LIBS']:
        print "missing imlib2, install it."
        exit(1)
    else:
        print "yes"
        ipager_env.AppendUnique(
            CPPPATH = imlib2_env.Dictionary()['CPPPATH'], 
            CCFLAGS = imlib2_env.Dictionary()['CCFLAGS'], 
            LIBPATH = imlib2_env.Dictionary()['LIBPATH'],
            LIBS    = imlib2_env.Dictionary()['LIBS']

        )

conf.Finish()


###################################################
# build app
ipager_env.Program('ipager', 
    [
        'atoms.cpp',
        'icolor.cpp',
        'icon.cpp',
        'iconfig.cpp',
        'ipager.cpp',
        'notifier.cpp',
        'pager.cpp',
        'wm.cpp',
        'workspace.cpp'	
    ]    
);

####################################################
# install
ipager_env.Install(ipager_instdir_bin, 'ipager')


####################################################
# alias
ipager_env.Alias('install', ipager_instdir)



####################################################
# help
Help(ipager_options.GenerateHelpText(ipager_env))




