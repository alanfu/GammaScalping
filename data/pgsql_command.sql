drop table if exists spyoptions;

create table spyoptions
	(
		cur_date date not null,
		ex_date date not null,
		flag char(1) not null,
		k numeric(12,5) not null,
		bid numeric(12,5) not null,
		offer numeric(12,5) not null,
		price numeric(12,5),
		primary key (cur_date, ex_date, flag, k)
	);

copy spyoptions (cur_date, ex_date, flag, k, bid, offer)
from '/Users/alanfu/Documents/Codes/cpp/GammaScalping/data/op_SPY.csv'
delimiter ',' csv header;

