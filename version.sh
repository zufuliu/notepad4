#!/bin/bash
# (C) 2012-2013 see Authors.txt
#
# This file is part of MPC-HC.
#
# MPC-HC is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 3 of the License, or
# (at your option) any later version.
#
# MPC-HC is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program. If not, see <https://www.gnu.org/licenses/>.

# This is the last svn changeset, the number and hash can be automatically
# calculated, but it is slow to do that. So it is better to have it hardcoded.
svnrev=990
svnhash="b27830c304eff6b778094307cfad005b6ac5f2f2"

versionfile="./src/VersionRev.h"
manifestfile="./res/Notepad2.exe.manifest"

# If we are not inside a git repo use hardcoded values
if ! git rev-parse --git-dir > /dev/null 2>&1; then
  hash=0000000
  ver=0
else
  # Get the current branch name
  branch=$(git symbolic-ref -q HEAD) && branch=${branch##refs/heads/} || branch="no branch"

  # If we are on another branch that isn't master, we want extra info like on
  # which commit from master it is based on and what its hash is. This assumes we
  # won't ever branch from a changeset from before the move to git
  if [[ "$branch" != "master" ]]; then
    version_info="#define BRANCH _T(\"$branch\")"$'\n'
    if git show-ref --verify --quiet refs/heads/master; then
      # Get where the branch is based on master
      base=$(git merge-base master HEAD)
      base_ver=$(git rev-list --count $svnhash..$base)
      base_ver=$((base_ver + svnrev))
      ver_full=" ($branch) (master@${base_ver:0:7})"
    else
      ver_full=" ($branch)"
    fi
  fi

  # Count how many changesets we have since the last svn changeset
  ver=$(git rev-list --count $svnhash..HEAD)
  # Now add it with to last svn revision number
  ver=$((ver + svnrev))

  # Get the abbreviated hash of the current changeset
  hash=$(git rev-parse --short HEAD)

fi

ver_full="TEXT(\"r$ver ($hash)$ver_full\")"

version_info+="#define VERSION_MINOR `date +%y`"$'\n'
version_info+="#define VERSION_BUILD `date +%m`"$'\n'
version_info+="#define VERSION_BUILD_NUM `date +%-m`"$'\n'
version_info+="#define VERSION_HASH TEXT(\"$hash\")"$'\n'
version_info+="#define VERSION_REV $ver"$'\n'
version_info+="#define VERSION_REV_FULL $ver_full"

if [[ "$branch" ]]; then
  echo "On branch: $branch"
fi
echo "Hash:      $hash"
if [[ "$branch" ]] && ! git diff-index --quiet HEAD; then
  echo "Revision:  $ver (Local modifications found)"
else
  echo "Revision:  $ver"
fi
if [[ -n "$base" ]]; then
  echo "Mergebase: master@${base_ver} (${base:0:7})"
fi

# Update VersionRev.h if it does not exist, or if version information was changed.
if [[ ! -f "$versionfile" ]] || [[ "$version_info" != "$(<"$versionfile")" ]]; then
  # Write the version information to VersionRev.h
  echo "$version_info" > "$versionfile"
fi

# Update manifest file if version information was changed.
base_ver="4"
new_ver="`date +%y.%m`.${ver}"
newmanifest="$(sed -Ee "0,/(${base_ver}\.)([0-9.]+)/s//\1${new_ver}/g" "$manifestfile")"
if [[ "$newmanifest" != "$(<"$manifestfile")" ]]; then
  # Update the revision number in the manifest file
  echo "$newmanifest" > "$manifestfile"
fi

# Update metapath's manifest and version information
if [[ $# -ne 0 ]]; then
  versionfile="./metapath/src/VersionRev.h"
  manifestfile="./metapath/res/metapath.exe.manifest"
  if [[ ! -f "$versionfile" ]] || [[ "$version_info" != "$(<"$versionfile")" ]]; then
    echo "$version_info" > "$versionfile"
  fi

  base_ver="4"
  newmanifest="$(sed -Ee "0,/(${base_ver}\.)([0-9.]+)/s//\1${new_ver}/g" "$manifestfile")"
  if [[ "$newmanifest" != "$(<"$manifestfile")" ]]; then
    echo "$newmanifest" > "$manifestfile"
  fi
fi
