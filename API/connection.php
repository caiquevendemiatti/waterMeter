<!doctype html>
    <html>  
        <head>
            <meta charset="utf-8">
            <title>Untitled Document</title>    
        </head>
            <?php  
            
            function DBEscape ($dados){//Function for avoiding SQL Injection
                $link = DBConnect();  //Connects with database
                if(!is_array($dados)){  //If data is not an array, it will only remove special charcters from the string
                    $dados = mysqli_real_escape_string ($link, $dados); 
                }
                else{ //If data is an array
                    $arr=$dados; 
                    foreach($arr as $key => $value){//Sweep entire array removing the special characters
                        $key = mysqli_real_escape_string ($link, $key);
                        $value = mysqli_real_escape_string ($link, $value);
                        $dados [$key] = $value; 
                    }
                }

                DBClose($link); //Close database connection
                //Returns the clean data
                return $dados; 
            }
            
            
            function DBConnect(){ //Function for connecting to database
                $link=@mysqli_connect(DB_HOSTNAME, DB_USERNAME,DB_PASSWORD, DB_DATABASE) //Tries to start the connection using previously defined parameters
                            or die(mysqli_connect_error()); //In case of error returns the error
                mysqli_set_charset($link, DB_CHARSET) or die(mysqli_error($link)); //Define database charset
                //Returns the connection variable
                return $link;
            }

            
            function DBClose($link){//Function for closing the database connection
                @mysqli_close($link) or die(mysqli_error($link)); 
            }
            ?>  
        <body> 
        </body>
    </html>