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
        "x86_64-linux-gnu": "debian:buster",
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
            "RUN apt-get update \\\n"
            "&& apt-get install -y sudo \\\n"
            "&& useradd -m steroids_user \\\n"
            "&& echo \"steroids_user:steroids_user\" | chpasswd \\\n"
            "&& adduser steroids_user sudo \\\n"
            "&& echo '%sudo ALL=(ALL) NOPASSWD:ALL' >> /etc/sudoers\n"
            "USER steroids_user \n"
            "WORKDIR /tmp/.steroids \n"
            "RUN sudo bash -c 'echo \"deb http://deb.debian.org/debian buster-backports main\" >> /etc/apt/sources.list' \\\n"
            "&& sudo apt-get update \\\n"
            "&& sudo apt-get install -y python3 python3-pip \\\n"
            "&& pip3 install requests bs4 \\\n"
            "&& mkdir -p ~/.vgazer/github \\\n"
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
            "WORKDIR /mnt/steroids \n"
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
