import errno
import os
import sys

from setuptools import Extension

from paver.easy import *
from paver.path import path
from paver.setuputils import setup


info = dict(
    name="more_zlib",
    description="fast, efficient serialization",
    version="0.1",
    author="Travis Parker",
    author_email="travis.parker@gmail.com",
)

if sys.subversion[0].lower() != 'pypy':
    info['ext_modules'] = [Extension(
        'more_zlib',
        [
            'more_zlib_module.c',
            'zlib/adler32.c',
            'zlib/compress.c',
            'zlib/crc32.c',
            'zlib/deflate.c',
            'zlib/gzio.c',
            'zlib/infback.c',
            'zlib/inffast.c',
            'zlib/inflate.c',
            'zlib/inftrees.c',
            'zlib/trees.c',
            'zlib/uncompr.c',
            'zlib/zutil.c'
        ],
        extra_compile_args=['-Wall'])]

setup(**info)

MANIFEST = (
    "setup.py",
    "paver-minilib.zip",
)

@task
def manifest():
    path('MANIFEST.in').write_lines('include %s' % x for x in MANIFEST)

@task
@needs('generate_setup', 'minilib', 'manifest', 'setuptools.command.sdist')
def sdist():
    pass

@task
def clean():
    for p in map(path, ('more_zlib.egg-info', 'dist', 'build', 'MANIFEST.in')):
        if p.exists():
            if p.isdir():
                p.rmtree()
            else:
                p.remove()
    for p in path(__file__).abspath().parent.walkfiles():
        if p.endswith(".pyc") or p.endswith(".pyo"):
            try:
                p.remove()
            except OSError, exc:
                if exc.args[0] == errno.EACCES:
                    continue
                raise
