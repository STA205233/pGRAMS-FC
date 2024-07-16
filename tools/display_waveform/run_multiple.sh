#! /bin/zsh
for file  in ~/data/daq/daq_output_000${1}_*_*.dat ;do
# echo $file
./build/display_waveform $file
done
