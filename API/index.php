<!DOCTYPE html>
<html>
    <head>
    </head>
    <body>
        <?php
            require 'config.php'; //Request another pieces of code
            require 'connection.php'; 
            require 'database.php';
            $action  = $_GET['action'];	
                    
            if($action=='write'){ //If "action" received query parameter index is "write", will read another query parameters received
                $nse = $_GET['nse']; //Sensor serial number
                $sensor1 = $_GET['sensor1']; 
                $sensor2 = $_GET['sensor2']; 
                $sensor3 = $_GET['sensor3'];
           
            $dados = array(  //Generate a data array for easy manipulation
                'nse' => $nse,
                'sensor1' => $sensor1,
                'sensor2' => $sensor2,
                'sensor3' => $sensor3,);
        
            var_dump($dados); //Respond with received data
            $grava= DBCreate('sensores', $dados);  //Call function for storing the data
            if($grava) { //Check if sucess on storing
	            echo "Dados armazenados";
            }
            else{ 
	            echo ":(";
            }
        }

        if($action=="read"){ //If "action" received query parameter index is "read", proceed a database consult
            $clientes = DBRead('sensores',"WHERE id !=0 ", 'sensor1');  //Read all data related to sensor1
            $total=count($clientes); 
            for($i=0; $i< $total; $i++ ){ //Returns the stored data
	            echo "<pre>"; 
                print_r($clientes[$i]); 
                echo "</pre>";		 
                }	
        }	
        ?> 
    </body>
</html>