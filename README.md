**Sepa2odoo**

A tool that integrates Odoo Sepa credit and ISO2002 payments with banking systems.

Features:
- upload bank statement and transactions to odoo: read and parse a bank statement (camt.053.001.02 xml file) and write entries to odoo (so they can be reconciled in odoo)
  (this is type XT for samlink type banks, type RA is also partly supported but disabled for now)
- read payment from odoo (SEPA Credit Transfer  and ISO2002 payments supported), convert to a Customer Credit Transfer Initiation file (pain.001.001.09 xml)
  (this is type XL for samlink type banks)
- upload payment status to odoo: read and parse a payment status notification (pain.002.001.10 xml file) and write status to odoo
  (this is type XP for samlink type banks)
- get payments from odoo and generate payment initiation xml

run this tool with:
sepa2odoo -c /etc/toolsconf/sepa2odoo.conf

This tool will work on files (root folder):
<pre>
./incoming              <- Read and process XP and XT files.
                           Files that are not recognized are moved to ./not_processed folder
./processed             <- Files that have been successfully processed are stored here
./not_processed         <- Unrecognized files are stored here

./outgoing              <- Store generated XL files in this folder
</pre>

For sending files to bank and downloading files see:
https://github.com/kdilayer/ws_sepa

Configuration 
<pre>
  {
    "working_folder": "/var/lib/ws_sepa",                          => Root folder for storing files
    "profiles": [
        {
            "name": "Company name",
            "iban": "Your iban account number",

            "odoo_url": "https://xxx-url-db-237848.dev.odoo.com/", => this is your odoo server url
            "odoo_db": "db-237848",                                => odoo database id
            "odoo_username": "my_user@mydomain.com",               => odoo user name 
            "odoo_api_key": "odoo api key",                        => odoo api key
            "odoo_company_id": 1                                   => odoo company id 
            
        },
        {
           ...
        }
        
    ]
    
}
</pre>

