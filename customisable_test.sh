clear
echo "TEST START!"

echo "Compile Code"

make data_transfer_server data_transfer_client

echo "Create Env Folders..."
mkdir -p client_folder
mkdir -p server_folder


num=$1
string=$2
filepath=$3
filename="$(basename $filepath)"  # Returns just "to"
filedir="$(dirname $filepath)"
cp -f $filepath ./client_folder/ 

echo "FILE TO SEND: "$filename
#echo "Generate Large File..."

echo "----------------------START SERVER!------------------------------"
cd server_folder
(../data_transfer_server &)

cd ..

echo "----------------------START CLIENT!------------------------------"
cd client_folder
../data_transfer_client -num "$num" -str "$str" -tfile ./$filename
#../data_transfer_client -num 100 -str "hola_amigo" -tfile ./$filename  
cd ..

echo ""
echo ""
echo ""
echo ""
echo ""
echo ""

echo "----------------------CHECKING "$filename" FILES------------------------------"
diff -rs "./client_folder/"$filename "./server_folder/"$filename | egrep '^Files .+ and .+ are identical$'


echo ""
echo ""
echo ""
echo ""
echo ""
echo ""



echo "----------------------KILL SERVER------------------------------"
killall data_transfer_server
rm -r server_folder/
rm -r client_folder/
make clean