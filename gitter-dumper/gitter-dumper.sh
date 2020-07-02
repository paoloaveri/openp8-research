#!/bin/bash
#
# Perform local backups of Gitter.im chat rooms.
# Credits to @dale3h (https://gist.github.com/dale3h/002b172393f8092296c154ffdbe4890e)
# This modified version also creates a total.html that contains everything in one file

GITTER_ARCHIVE_URL="https://gitter.im/%s/archives/%s"

usage() {
  echo "usage: $(basename $0) -f -r <chat-room> [-o <output-dir>] [-s <YYYY/MM/DD>] [-e <YYYY/MM/DD>]" 1>&2
  exit 1
}

err() {
  echo "$@" >&2
  return 1
}

force_write=false

while getopts ":r:o:s:e:f" opt; do
  case $opt in
    r  ) chat_room="$OPTARG";;
    o  ) output_dir="$OPTARG";;
    s  ) start_date="$OPTARG";;
    e  ) end_date="$OPTARG";;
    f  ) force_write=true;;
    \? ) err "Unknown option: -$OPTARG"; usage;;
    :  ) err "Missing option argument for -$OPTARG"; usage;;
    *  ) err "Unimplemented option: -$OPTARG"; usage;;
  esac
done

if [ -z "$chat_room" ]; then
  err "Missing required argument -r"
  usage
fi

if [ -z "$output_dir" ]; then
  output_dir="$chat_room"
fi

if [ -z "$start_date" ]; then
  start_date="$(date -j -v-7d "+%Y/%m/%d")"
else
  test_date="$(date -j -f "%Y/%m/%d" "$start_date" "+%Y/%m/%d" 2>/dev/null)"
  [[ $? -eq 0 ]] && start_date="$test_date" || { err "Illegal start date format: $start_date"; usage; }
fi

if [ -z "$end_date" ]; then
  end_date="$(date -j -v+1d "+%Y/%m/%d")"
else
  test_date="$(date -j -f "%Y/%m/%d" "$end_date" "+%Y/%m/%d" 2>/dev/null)"
  [[ $? -eq 0 ]] && end_date="$test_date" || { err "Illegal start date format: $end_date"; usage; }
fi

echo "Backing up chat logs for $chat_room between $start_date and $end_date"
echo "Output directory set to $output_dir"
echo

today="$(date -j "+%Y/%m/%d")"
d="$start_date"

wrote_something=false

while [[ ! "$d" > "$end_date" ]]; do
  output_file="$output_dir/$d.html"

  if $force_write || [[ ! "$d" < "$today" || ! -f "$output_file" ]]; then
    wrote_something=true
    echo "[$chat_room] $d"
    curl -s $(printf "$GITTER_ARCHIVE_URL" "$chat_room" "$d") --create-dirs -o "$output_file"
    xmllint --html --xpath "//div[@id='chat-container']" $output_file 2> /dev/null >> "$output_dir/total.html"
  fi

  d="$(date -j -v+1d -f "%Y/%m/%d" "$d" "+%Y/%m/%d")"
done

if ! $wrote_something; then
  echo "All logs already exist. Use option -f to force overwrite."
  exit 0
fi

echo
echo "Done"
