<!DOCTYPE html>
<html>
    <head>
        <meta charset="utf-8">
        <title>Untitled Document</title>
    </head>
    <?php
        function DBRead($table, $params, $fields){//Function for reading the stored data
            $table=DB_PREFIX.'_'.$table;  //Prepare the table name
            $params =($params) ? " {$params}" : null; //If no parameters were received set as null
            $query = "SELECT {$fields} FROM {$table}{$params}"; //Generates the query
            $result = DBExecute($query); //Call function for executing the query

            if(!mysqli_num_rows($result)){ //If there is no data stored do nothing
                return false;
            }
            else{//If there is stored data, generate an data array and returns the array
                while($res = mysqli_fetch_assoc($result)){
                    $data[]=$res;
                }
                return $data;	
            }
            
        }
        
        function DBCreate ($table, array $data){//Function for storing data into database
            $table=DB_PREFIX.'_'.$table;  //Prepare the table name
            $data = DBEscape($data); //Clean special characters
            //Explode the array, and turn it into a string sorting the informations using quotes and commas
            $fields = implode(', ', array_keys($data)); 
            $values = "'".implode("', '", $data)."'"; 
           
            $query= "INSERT INTO {$table} ({$fields}) VALUES ({$values})"; //Generate the insertion query
            return DBExecute($query); //Call function for executing the query

        }

        function DBExecute($query){ //Function for runnig queries
            $link=DBConnect ();  //Connect to database
           $result =@mysqli_query($link, $query) or die (mysqli_error($link));  //Run the query
            DBClose($link);  //Close the connection
            return $result; //Return the result of query
        }
    ?> 
    <body>
    </body>
</html>