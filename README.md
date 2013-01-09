uimi
====

Dbus based dictionary system <br/>

Client <--- dbus --> Daemon <br/>

<h2>Client</h2>
<ul>
<li>Programming language: don't care <br/></li>
</ul>

<h2>Daemon</h2>
<ul>
<li>Programming language: C <br/></li>
<li>Feature:<br/>
<ul>
<li>Multiple dictionary support (Dbus object path) </li>
<li>Plug-in support </li>
<li>Various data support (DB, HTTP Request, Custom file, ...) </li>
</ul>
</li>
</ul>

<h2>Dbus introspection</h2>
<ul>

<li>Manager:
<ul>
<li>Service: "org.uimi"</li>
<li>Object path: "/org/uimi"</li>
<li>Interface: "org.uimi.Manager"</li>
<li>Method: "GetDictionaries()"</li>
</ul>
</li>

<li>Dictionary - XXX:
<ul>
<li>Object path: "/org/uimi/XXX"</li>
<li>Interface: "org.uimi.dict"</li>
<li>Method: "Search()"</li>
</ul>
</li>

</ul>

<h2>Configuration</h2>
<ul>
<li>File:
<ul>
<li>File-1: /etc/uimi.conf</li>
<li>File-2: ~/.uimi.conf</li>
</ul>
<li>Contents:
<pre>
[XXX]
driver=sqlite3
parameter=~/word.db

[YYY]
driver=http
parameter=http://your-service/?={$1}

[ZZZ]
driver=my_custom_plugin
parameter=~/my_scrap.dat

</pre>
</ul>
