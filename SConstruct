env = Environment(CPPPATH = '.')
src = Glob('*.c')
env.Program('alarm',
		src,
        	CCFLAGS = '-DHELLOSCONS',
		CFLAGS = ['-g',
			'-fcolor-diagnostics',
			'-fpack-struct',
			'-fno-strict-aliasing',
			'-Wno-unused-variable',
			'-Os',
			'-Wall'],
		CC = 'clang')
