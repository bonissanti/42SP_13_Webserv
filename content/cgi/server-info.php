#!/usr/bin/env php
<?php
echo "<html>";
echo "<head>";
echo "<title>Informações do Servidor</title>";
echo "<style>";
echo "body { font-family: Arial, sans-serif; background-color: #f0f0f0; text-align: center; padding: 50px; }";
echo "h1 { color: #333; }";
echo "p { font-size: 1.2em; color: #666; }";
echo ".container { background-color: #fff; padding: 20px; border-radius: 10px; box-shadow: 0 0 10px rgba(0, 0, 0, 0.1); display: inline-block; }";
echo "</style>";
echo "</head>";
echo "<body>";
echo "<div class='container'>";
echo "<h1>Informações do Servidor</h1>";
echo "<p>Nome do Servidor: " . $_SERVER['SERVER_NAME'] . "</p>";
echo "<p>Porta: " . $_SERVER['SERVER_PORT'] . "</p>";
echo "<p>Protocolo: " . $_SERVER['SERVER_PROTOCOL'] . "</p>";
echo "<p>Caminho do arquivo: " . $_SERVER['PATH_INFO'] . "</p>"; 
echo "<p>Método solicitado: " . $_SERVER['REQUEST_METHOD'] . "</p>";
echo "<p>Gateway interface: " . $_SERVER['GATEWAY_INTERFACE'] . "</p>"; 
echo "</div>";
echo "</body>";
echo "</html>";
?>
