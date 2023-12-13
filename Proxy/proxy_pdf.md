Proxy Lab: Writing a Caching Web Proxy
Assigned: Thu, Nov 19
Due: Tue, Dec 8, 11:59 PM
Last Possible Time to Turn In: Fri, Dec 11, 11:59 PM

# 1 Introduction
A Web proxy is a program that acts as a middleman between a Web browser and an end server. Instead of
contacting the end server directly to get a Web page, the browser contacts the proxy, which forwards the
request on to the end server. When the end server replies to the proxy, the proxy sends the reply on to the
browser.
Proxies are useful for many purposes. Sometimes proxies are used in firewalls, so that browsers behind a
firewall can only contact a server beyond the firewall via the proxy. Proxies can also act as anonymizers:
by stripping requests of all identifying information, a proxy can make the browser anonymous to Web
servers. Proxies can even be used to cache web objects by storing local copies of objects from servers then
responding to future requests by reading them out of its cache rather than by communicating again with
remote servers.
In this lab, you will write a simple HTTP proxy that caches web objects. For the first part of the lab, you will
set up the proxy to accept incoming connections, read and parse requests, forward requests to web servers,
read the servers’ responses, and forward those responses to the corresponding clients. This first part will
involve learning about basic HTTP operation and how to use sockets to write programs that communicate
over network connections. In the second part, you will upgrade your proxy to deal with multiple concurrent
connections. This will introduce you to dealing with concurrency, a crucial systems concept. In the third
and last part, you will add caching to your proxy using a simple main memory cache of recently accessed
web content.