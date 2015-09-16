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
			'-O0',
			'-Wall'],
		CC = 'clang')
