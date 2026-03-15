for i in $(find assets -type f -exec realpath -s --relative-to=assets {} ';'); do
    echo "Compiling asset \"$i\"" >&2
    BYTES=$(cat assets/$i | hexdump -v -e '1/1 "0x%02X,"')
    COUNT=$(wc -c assets/$i | awk '{print $1}')
    echo "{ \"$i\", $COUNT, (uint8_t[]){ $BYTES }},"
done