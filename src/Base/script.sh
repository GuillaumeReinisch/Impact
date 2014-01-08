for f in *.h
do echo "Processing $f file.."
   sed -i '' 's/atomism/impact/g' $f
   sed -i '' 's/paKineticMechanism/msKineticMechanism/g' $f
   sed -i '' 's/paMicroscopicPhase/msMicroscopicPhase/g' $f
   sed -i '' 's/paNasa7CoefsTD/msNasa7CoefsTD/g' $f
   sed -i '' 's/paPhase/msPhase/g' $f
   sed -i '' 's/paReaction/msReaction/g' $f
   sed -i '' 's/paSpecie/msSpecie/g' $f
   sed -i '' 's/paThermoPhase/msThermoPhase/g' $f
   sed -i '' 's/atomism/impact/g' $f
   sed -i '' 's/phenolic_ablation/cantera/g' $f
done


