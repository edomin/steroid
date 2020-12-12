#!/usr/bin/env python3

import os
from pathlib import Path
import argparse

def GetDepsfileName(target):
    return {
        "x86_64-linux-gnu": "deps/x86_64-linux-gnu",
    }[target]

def GetDockefrileName(target):
    return {
        "x86_64-linux-gnu": "dockerfiles/deps-x86_64-linux-gnu.dockerfile",
    }[target]

def GetBaseImageName(target):
    return {
        "x86_64-linux-gnu": "vgazer_min_env_x86_64_debian_buster",
    }[target]

def GetTargetArch(target):
    return {
        "x86_64-linux-gnu": "x86_64",
    }[target]

def GetTargetOs(target):
    return {
        "x86_64-linux-gnu": "linux",
    }[target]

def GetTargetAbi(target):
    return {
        "x86_64-linux-gnu": "gnu",
    }[target]

def GetDeps(filename):
    with open(filename) as f:
        return f.read().splitlines()

def GetGithubUsername():
    with open(
        os.path.join(str(Path.home()), ".vgazer/github/username"), "r"
    ) as f:
        return f.read()

def GetGithubToken():
    with open(
        os.path.join(str(Path.home()), ".vgazer/github/token"), "r"
    ) as f:
        return f.read()

def GenerateDockerfile(filename, baseImageName, targetArch, targetOs,
 targetAbi, deps):
    with open(filename, "w") as f:
        f.write(
            "FROM {baseImageName} as build \n"
            "MAINTAINER Vasiliy Edomin <Vasiliy.Edomin@gmail.com> \n"
            "USER vgazer_user \n"
            "WORKDIR /tmp/.mtr \n"
            "RUN mkdir -p ~/.vgazer/github \\\n"
            "&& echo \"{username}\" >> ~/.vgazer/github/username \\\n"
            "&& echo \"{token}\" >> ~/.vgazer/github/token \\\n"
            "&& python3 -m pip install --index-url https://test.pypi.org/simple/ --no-deps vgazer \\\n"
            "&& sudo sh -c 'echo \"Defaults env_keep += \\\"PYTHONPATH\\\"\" >> /etc/sudoers' \\\n"
            "&& sudo sh -c 'echo \"#!/usr/bin/env python3\" >> ./deps_installer.py' \\\n"
            "&& sudo sh -c 'echo \"from vgazer.vgazer import Vgazer\" >> ./deps_installer.py' \\\n"
            "&& sudo sh -c 'echo \"deps=[\" >> ./deps_installer.py' \\\n"
            "".format(baseImageName=baseImageName, username=GetGithubUsername(),
                token=GetGithubToken())
        )
        for dep in deps:
            f.write(
                "&& sudo sh -c 'echo \"\\\"{dep}\\\",\" >> ./deps_installer.py' \\\n"
                "".format(dep=dep)
            )
        f.write(
            "&& sudo sh -c 'echo \"]\" >> ./deps_installer.py' \\\n"
            "&& sudo sh -c 'echo \"gazer = Vgazer(arch=\\\"{arch}\\\", os=\\\"{os}\\\", osVersion=\\\"any\\\", abi=\\\"{abi}\\\")\" >> ./deps_installer.py' \\\n"
            "&& sudo sh -c 'echo \"gazer.InstallList(deps, verbose=True)\" >> ./deps_installer.py' \\\n"
            "&& sudo chmod u+x ./deps_installer.py \\\n"
            "&& sudo -E sh -c 'python3 -u ./deps_installer.py' \n"
            "WORKDIR /steroids \n"
            "".format(baseImageName=baseImageName,
                username=GetGithubUsername(),
                token=GetGithubToken(), arch = targetArch, os = targetOs,
                abi = targetAbi)
        )


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("--target")
    args = parser.parse_args()
    depsFile = GetDepsfileName(args.target)
    deps = GetDeps(depsFile)
    dockerfileName = GetDockefrileName(args.target)
    baseImageName = GetBaseImageName(args.target)
    targetArch = GetTargetArch(args.target)
    targetOs = GetTargetOs(args.target)
    targetAbi = GetTargetAbi(args.target)
    GenerateDockerfile(dockerfileName, baseImageName, targetArch, targetOs,
        targetAbi, deps)

if __name__ == "__main__":
    main()
