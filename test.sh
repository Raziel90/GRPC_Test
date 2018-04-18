clear
echo "TEST START!"

echo "Compile Code"

make data_transfer_server data_transfer_client

echo "Create Env Folders..."
mkdir -p client_folder
mkdir -p server_folder

filepath=./lipsum.txt
filename="$(basename $filepath)"

cp -rf $filepath client_folder/ 

echo $filename
#echo "Generate Large File..."
#dd if=/dev/zero of=./client_folder/file.txt count=101024 bs=1024 #Generates a big file to send
echo "----------------------START SERVER!------------------------------"
cd server_folder
(../data_transfer_server &)
#SERVER_PID=$!
cd ..

echo "----------------------START CLIENT!------------------------------"
cd client_folder
../data_transfer_client -num 100 -str "hola amigo" -file ./$filename 
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
rm -r server_folder
rm -r client_folder
make clean