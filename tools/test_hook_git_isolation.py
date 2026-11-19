import os
from pathlib import Path
import shlex
import subprocess
import sys
import tempfile
import unittest


HOOK = Path(__file__).resolve().parent / 'hooks/tool-tests.sh'
FIXTURE = '''import pathlib, subprocess, tempfile, unittest

class RepositoryFixture(unittest.TestCase):
    def test_child_repository(self):
        with tempfile.TemporaryDirectory() as directory:
            root = pathlib.Path(directory).resolve()
            subprocess.run(['git', 'init', '-q', str(root)], check=True)
            actual = subprocess.check_output(['git', 'rev-parse', '--show-toplevel'], cwd=root, text=True).strip()
            self.assertEqual(pathlib.Path(actual).resolve(), root)
            (root / 'child.txt').write_text('fixture')
            subprocess.run(['git', 'add', '.'], cwd=root, check=True)
            subprocess.run(['git', '-c', 'user.name=test', '-c', 'user.email=1+test@users.noreply.github.com',
                            'commit', '-qm', 'child fixture'], cwd=root, check=True)
'''


class HookGitIsolationTests(unittest.TestCase):
    def test_repository_fixtures_cannot_change_committing_checkout(self):
        environment = os.environ.copy()
        environment['PYTHONDONTWRITEBYTECODE'] = '1'
        for name in subprocess.check_output(['git', 'rev-parse', '--local-env-vars'], text=True).splitlines():
            environment.pop(name, None)
        with tempfile.TemporaryDirectory() as directory:
            root = Path(directory).resolve()
            binary = root / 'bin'
            binary.mkdir()
            uv = binary / 'uv'
            uv.write_text('#!/bin/sh\nshift\nshift\nexec ' + shlex.quote(sys.executable) + ' "$@"\n')
            uv.chmod(0o755)
            environment['PATH'] = str(binary) + os.pathsep + environment.get('PATH', '')

            def git(where, *args):
                result = subprocess.run(['git', *args], cwd=where, env=environment,
                                        stdout=subprocess.PIPE, stderr=subprocess.STDOUT, text=True)
                self.assertEqual(result.returncode, 0, result.stdout)
                return result.stdout.strip()

            for linked in (False, True):
                with self.subTest(linked_worktree=linked):
                    repo = root / ('linked' if linked else 'ordinary')
                    repo.mkdir()
                    git(repo, 'init', '-q')
                    git(repo, 'config', 'user.name', 'test')
                    git(repo, 'config', 'user.email', '1+test@users.noreply.github.com')
                    (repo / 'sentinel').write_text('keep this file')
                    git(repo, 'add', '.')
                    git(repo, 'commit', '-qm', 'initial')
                    before = git(repo, 'rev-parse', 'HEAD')
                    checkout = repo
                    if linked:
                        checkout = root / 'checkout'
                        git(repo, 'worktree', 'add', '-qb', 'topic', str(checkout))
                    hooks = root / ('linked-hooks' if linked else 'ordinary-hooks')
                    hooks.mkdir()
                    hook = hooks / 'pre-commit'
                    hook.write_bytes(HOOK.read_bytes())
                    hook.chmod(0o755)
                    git(checkout, 'config', 'core.hooksPath', str(hooks))
                    (checkout / 'tools').mkdir()
                    (checkout / 'tools/test_fixture.py').write_text(FIXTURE)
                    git(checkout, 'add', 'tools/test_fixture.py')
                    git(checkout, 'commit', '-qm', 'add fixture')
                    self.assertEqual(git(checkout, 'show', '-s', '--format=%s'), 'add fixture')
                    self.assertEqual(git(checkout, 'rev-parse', 'HEAD^'), before)
                    self.assertEqual(git(checkout, 'ls-tree', '-r', '--name-only', 'HEAD').splitlines(),
                                     ['sentinel', 'tools/test_fixture.py'])
                    self.assertEqual(git(checkout, 'status', '--porcelain'), '')


if __name__ == '__main__':
    unittest.main()
