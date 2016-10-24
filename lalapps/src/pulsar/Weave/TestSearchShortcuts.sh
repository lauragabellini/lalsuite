# Perform an interpolating search without/with part of the search being shortcut, and check for consistent results

echo "=== Create search setup with 4 segments ==="
set -x
${builddir}/lalapps_WeaveSetup --first-segment=1122332211/90000 --segment-count=4 --detectors=H1,L1 --output-file=WeaveSetup.fits
set +x
echo

echo "=== Perform interpolating search ==="
set -x
${builddir}/lalapps_Weave --output-file=WeaveOutNoShort.fits \
    --output-toplist-limit=5000 --output-misc-info --setup-file=WeaveSetup.fits \
    --sft-timebase=1800 --sft-noise-psd=1,1 --sft-noise-rand-seed=3456 \
    --sft-timestamps-files=${srcdir}/timestamps-irregular.txt,${srcdir}/timestamps-regular.txt \
    --injections="{h0=0.5; cosi=0.2; psi=0.4; phi0=0.1; Alpha=2.72; Delta=-0.38; Freq=50.5; f1dot=-1e-9}" \
    --alpha=2.7/0.05 --delta=-0.4/0.05 --freq=50.5/1e-4 --f1dot=-1e-8,0 --semi-max-mismatch=0.6 --coh-max-mismatch=0.3
set +x
echo

echo "=== Perform interpolating search with all computations shortcut ==="
set -x
${builddir}/lalapps_Weave --shortcut-compute --output-file=WeaveOutShortComp.fits \
    --output-toplist-limit=5000 --output-misc-info --setup-file=WeaveSetup.fits \
    --sft-timebase=1800 --sft-noise-psd=1,1 --sft-noise-rand-seed=3456 \
    --sft-timestamps-files=${srcdir}/timestamps-irregular.txt,${srcdir}/timestamps-regular.txt \
    --injections="{h0=0.5; cosi=0.2; psi=0.4; phi0=0.1; Alpha=2.72; Delta=-0.38; Freq=50.5; f1dot=-1e-9}" \
    --alpha=2.7/0.05 --delta=-0.4/0.05 --freq=50.5/1e-4 --f1dot=-1e-8,0 --semi-max-mismatch=0.6 --coh-max-mismatch=0.3
set +x
echo

echo "=== Perform interpolating search with main search loop shortcut ==="
set -x
${builddir}/lalapps_Weave --shortcut-search --output-file=WeaveOutShortSearch.fits \
    --output-toplist-limit=5000 --output-misc-info --setup-file=WeaveSetup.fits \
    --sft-timebase=1800 --sft-noise-psd=1,1 --sft-noise-rand-seed=3456 \
    --sft-timestamps-files=${srcdir}/timestamps-irregular.txt,${srcdir}/timestamps-regular.txt \
    --injections="{h0=0.5; cosi=0.2; psi=0.4; phi0=0.1; Alpha=2.72; Delta=-0.38; Freq=50.5; f1dot=-1e-9}" \
    --alpha=2.7/0.05 --delta=-0.4/0.05 --freq=50.5/1e-4 --f1dot=-1e-8,0 --semi-max-mismatch=0.6 --coh-max-mismatch=0.3
set +x
echo

echo "=== Check approximate/computed number of semicoherent templates are consistent ==="
set -x
${fitsdir}/lalapps_fits_header_getval "WeaveOutNoShort.fits[0]" 'SEMIAPPX' > tmp
semi_ntmpl_no_short=`cat tmp | xargs printf "%d"`
${fitsdir}/lalapps_fits_header_getval "WeaveOutShortComp.fits[0]" 'SEMIAPPX' > tmp
semi_ntmpl_short_comp=`cat tmp | xargs printf "%d"`
${fitsdir}/lalapps_fits_header_getval "WeaveOutShortSearch.fits[0]" 'SEMIAPPX' > tmp
semi_ntmpl_short_search=`cat tmp | xargs printf "%d"`
[ ${semi_ntmpl_no_short} -eq ${semi_ntmpl_short_comp} ]
[ ${semi_ntmpl_no_short} -eq ${semi_ntmpl_short_search} ]
${fitsdir}/lalapps_fits_header_getval "WeaveOutNoShort.fits[0]" 'SEMICOMP' > tmp
semi_ncomp_no_short=`cat tmp | xargs printf "%d"`
${fitsdir}/lalapps_fits_header_getval "WeaveOutShortComp.fits[0]" 'SEMICOMP' > tmp
semi_ncomp_short_comp=`cat tmp | xargs printf "%d"`
[ ${semi_ncomp_no_short} -eq ${semi_ncomp_short_comp} ]
set +x
echo

echo "=== Check peak memory usage is consistent ==="
set -x
${fitsdir}/lalapps_fits_header_getval "WeaveOutNoShort.fits[0]" 'PEAKMEM' > tmp
peak_mem_no_short=`cat tmp | xargs printf "%.5g"`
${fitsdir}/lalapps_fits_header_getval "WeaveOutShortComp.fits[0]" 'PEAKMEM' > tmp
peak_mem_short_comp=`cat tmp | xargs printf "%.5g"`
[ `echo ${peak_mem_no_short} ${peak_mem_short_comp} | awk '{ print 0.95 < $2/$1 && $2/$1 < 1.05 }'` -eq 1 ]
set +x
echo

echo "=== Check miscellaneous per-segment information is consistent ==="
set -x
${fitsdir}/lalapps_fits_table_list "WeaveOutNoShort.fits[per_seg_info]" > per_seg_info_no_short
${fitsdir}/lalapps_fits_table_list "WeaveOutShortComp.fits[per_seg_info]" > per_seg_info_short_comp
diff per_seg_info_no_short per_seg_info_short_comp
set +x
echo
