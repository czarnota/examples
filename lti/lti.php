<?php

include __DIR__ . '/vendor/autoload.php';

$server = new OAuthServer(new InMemoryOAuthDataStore([
    'wizut' => '12345'
]));

$server->add_signature_method(new OAuthSignatureMethod_HMAC_SHA1());

$request = OAuthRequest::from_request();

try {
    $server->verify_request($request);
} catch (OAuthException $e) {
    http_response_code(403);
    echo sprintf("403 Forbidden (%s)", $e->getMessage());
    die();
}


if ($request->get_parameter('lti_message_type') !== 'basic-lti-launch-request') {
    http_response_code(400);
    echo "400 Bad request";
    die();
}


function e($var) {
    return htmlspecialchars($var, ENT_QUOTES, 'UTF-8');
}

?>

<!doctype html>
<html>
<head>
    <meta charset="utf-8"/>
</head>
<body>
    <h1>Witaj</h1>
    <ul>
        <li>Id kursu: <?php echo e($request->get_parameter('context_id')); ?></li>
        <li>Kurs: <?php echo e($request->get_parameter('context_title')); ?></li>
        <li>Identyfikator kursu:<?php echo e($request->get_parameter('context_label')); ?></li>
        <li>Imie: <?php echo e($request->get_parameter('lis_person_name_given')); ?></li>
        <li>Nazwisko: <?php echo e($request->get_parameter('lis_person_name_family')); ?></li>
        <li>Email: <?php echo e($request->get_parameter('lis_person_contact_email_primary')); ?></li>
    </ul>

    <h2>Pozostałe parametry, przesłane wraz z zapytaniem</h2>
    <table>
        <thead>
            <tr>
                <th>Parametr</th>
                <th>Wartość</th>
            </tr>
        </thead>
        <tbody>
            <?php foreach ($request->get_parameters() as $key => $value): ?>
                <tr>
                    <td><?php echo e($key); ?></td>
                    <td><?php echo e($value); ?></td>
                </tr>
            <?php endforeach ?>
        </tbody>
    </table>
</body>
</html>
