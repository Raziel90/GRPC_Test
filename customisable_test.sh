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

cp -rf $filepath client_folder/ 

echo "FILE TO SEND: "$filename
#echo "Generate Large File..."

echo "----------------------START SERVER!------------------------------"
cd server_folder
(../data_transfer_server &)

cd ..

echo "----------------------START CLIENT!------------------------------"
cd client_folder
../data_transfer_client -num $num -str $str -file ./$filename 
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
rm server_folder/$filename
make clean