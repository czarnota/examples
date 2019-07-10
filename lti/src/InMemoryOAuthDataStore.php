<?php

class InMemoryOAuthDataStore extends OAuthDataStore
{
    private $consumers = [];

    public function __construct(array $consumers = []) 
    {
        $this->consumers = $consumers;
    }

    public function addConsumer($key, $secret) 
    {
        $this->consumers[$key] = $secret;
    }

    public function lookup_consumer($key) 
    {
        return isset($this->consumers[$key]) ? new OAuthConsumer($key, $this->consumers[$key], null) : null;
    }

    public function lookup_token($consumer, $token_type, $token)
    {
        return new OAuthToken($consumer, "");
    }

    public function lookup_nonce($consumer, $token, $nonce, $timestamp)
    {
        return null;
    }

    public function new_request_token($consumer, $callback = null)
    {
        return null;
    }

    public function new_access_token($token, $consumer, $verifier = null)
    {
        // return a new access token attached to this consumer
        // for the user associated with this token if the request token
        // is authorized
        // should also invalidate the request token
        return null;
    }
}
