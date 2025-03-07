<?php
// Database credentials
$servername = "localhost";  // Your database server (e.g., localhost)
$username = "root";         // Your MySQL username
$password = "";             // Your MySQL password
$dbname = "AutomaticAlcoholDetectionSys"; // Your database name

// Create connection
$conn = new mysqli($servername, $username, $password, $dbname);

// Check connection
if ($conn->connect_error) {
    die("Connection failed: " . $conn->connect_error);
}

// Get location and NIC from the GET request
$location = isset($_GET['location']) ? $_GET['location'] : '';
$nic = isset($_GET['nic']) ? $_GET['nic'] : '';

// Validate location data
if (!empty($location)) {
    // Split the location into latitude and longitude
    list($latitude, $longitude) = explode(',', $location);

    // Prepare and bind
    $stmt = $conn->prepare("INSERT INTO gps_data (nic, latitude, longitude) VALUES (?, ?, ?)");
    $stmt->bind_param("sss", $nic, $latitude, $longitude);

    // Execute the query
    if ($stmt->execute()) {
        echo "Data successfully saved!";
    } else {
        echo "Error: " . $stmt->error;
    }

    // Close the statement
    $stmt->close();
} else {
    echo "No location data received.";
}

// Close connection
$conn->close();
?>
