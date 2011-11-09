from glob import glob

env = Environment(YACCFLAGS=['-d'],
                  CPPPATH=['./', 'sparse/'],
                  CPPFLAGS=['-Wall', '-ggdb3', '-O2',
                            '-march=native'],
                  CC='g++')

env.Command('sparse/scm_token.h', # out
            'sparse/scm_token.l', # in
            'flex --header-file=sparse/scm_token.h sparse/scm_token.l')

env.Program('main-c',
            glob('sparse/*.l') + glob('sparse/*.y') + glob('*.cpp'))

