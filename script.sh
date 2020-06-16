#!/bin/bash

#g++ -g B01_node_generator_v01.cpp -o E01_ng.out
#g++ -g B02_channel_assignment_v02.cpp -o E02_cha.out
#g++ -g B03_power_control_cppch_v03.cpp -o E03_pccppch.out
#g++ -g B04_power_control_optch_v01.cpp -o E04_pcoptch.out
#g++ -g B06_gpcch_cpp_v03.cpp -o E06_gpcch.out
g++ -g B04_d2bs_opt_mediatior.cpp -o E04_med.out

for k in `seq 2 2`;
do
     	for n in `seq 10 10`;
	do	
		#echo "k=$k,n=$n" >> R01_cpp_SoP.txt
		echo "k=$k,n=$n" >> R01_opt_SoP.txt
		#echo "k=$k,n=$n" >> R01_opt+cpp_SoP.txt
		#echo "k=$k,n=$n" >> R01_game_SoP.txt
		#echo "k=$k,n=$n" >> R02_cpp_SoS.txt
		#echo "k=$k,n=$n" >> R02_opt_SoS.txt
		#echo "k=$k,n=$n" >> R02_game_SoS.txt
		#echo "k=$k,n=$n" >> R04_cpp_chat.txt
		#echo "k=$k,n=$n" >> R04_cpp_pct.txt
		echo "k=$k,n=$n" >> R04_opt_chpct.txt
		#echo "k=$k,n=$n" >> R04_game_t.txt
		#echo "k=$k,n=$n" >> R05_opt_verification.txt

		for itr in `seq 1 50`;
		do	
			echo "k=$k,n=$n,iteration=$itr"	
			#sleep 1
			
			sed -n "/^k=$k,n=$n,iteration=$itr$/,/^k=$k,n=$n,iteration=$(($itr + 1))$/p" R03_LoN.txt | awk '/^x/'|sed 's/x\[0\] = 5000;//g'|sed '/^\s*$/d'|sed 's/;//g'|sed 's/x//g'|sed 's/ = / /g'|sed 's/\[//g'|sed 's/\]//g'>S04_x.txt
			echo "$(($n + 1)) 5000" >> S04_x.txt
			sed -n "/^k=$k,n=$n,iteration=$itr$/,/^k=$k,n=$n,iteration=$(($itr + 1))$/p" R03_LoN.txt | awk '/^y/'|sed 's/y\[0\] = 5000;//g'|sed '/^\s*$/d'|sed 's/;//g'|sed 's/y//g'|sed 's/ = / /g'|sed 's/\[//g'|sed 's/\]//g'>S05_y.txt
			echo "$(($n + 1)) 5000" >> S05_y.txt
				
			#echo "k=$k,n=$n,iteration=$itr" >> R03_LoN.txt	
		
			#./E01_ng.out $n
			#./E02_cha.out $n $k
			#./E03_pccppch.out $n $k
			#./E06_gpcch.out $n $k
			./E04_med.out $n $k
		
			./scip.spx -f B05_pccha_centralized_opt.zpl >log.out 
			grep "^Solving Time (sec)" log.out | awk '{print $5}' >> R04_opt_chpct.txt
			grep "^objective value:" log.out | awk '{print $3}' >> R01_opt_SoP.txt
			#grep "^x#" log.out | sed 's/x#//g' | sed 's/#/ /g' | awk '{print $1" "$2" "$3}' > S06_opt_ch.txt
			#grep "^p#" log.out | sed 's/p#//g' | sed 's/#/ /g' | awk '{print $1}'| paste -sd+ | bc >> R05_opt_verification.txt
			#./E04_pcoptch.out $n $k

			rm S01_set_N.txt
			rm S01_set_A.txt
			rm S02_set_K.txt
			rm S03_nn.txt
			rm S03_sir.txt
			rm S04_x.txt
			rm S05_y.txt
			#rm S06_opt_ch.txt
			#rm S06_cpp_ch.txt
			rm log.out
		done 
	done    
done 

#rm E01_ng.out
#rm E02_cha.out
#rm E03_pccppch.out
#rm E04_pcoptch.out
#rm E06_gpcch.out
rm E04_med.out
