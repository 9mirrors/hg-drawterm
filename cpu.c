/*
 * cpu.c - Make a connection to a cpu server
 *
 *	   Invoked by listen as 'cpu -R | -N service net netdir'
 *	    	   by users  as 'cpu [-h system] [-c cmd args ...]'
 */

#include <u.h>
#include <libc.h>
#include <auth.h>
#include <fcall.h>
#include <authsrv.h>
#include <libsec.h>
#include "args.h"
#include "drawterm.h"

#define Maxfdata 8192
#define MaxStr 128

static void	fatal(int, char*, ...);
static void	usage(void);
static void	writestr(int, char*, char*, int);
static int	readstr(int, char*, int);
static char	*rexcall(int*, char*, char*);
static char *keyspec = "";
static AuthInfo *p9any(int);

#define system csystem
static char	*system;
static int	cflag;
extern int	dbg;
extern char*   base;   // fs base for devroot 

static char	*srvname = "ncpu";
static char	*ealgs = "rc4_256 sha1";

/* message size for exportfs; may be larger so we can do big graphics in CPU window */
static int	msgsize = Maxfdata+IOHDRSZ;

/* authentication mechanisms */
static int	p9auth(int);
static int	srvp9auth(int, char*);

char *authserver;

typedef struct AuthMethod AuthMethod;
struct AuthMethod {
	char	*name;			/* name of method */
	int	(*cf)(int);		/* client side authentication */
	int	(*sf)(int, char*);	/* server side authentication */
} authmethod[] =
{
	{ "p9",		p9auth,		srvp9auth,},
	{ 0 }
};
AuthMethod *am = authmethod;	/* default is p9 */

char *p9authproto = "p9any";

int setam(char*);

void
exits(char *s)
{
	print("\ngoodbye\n");
	for(;;) osyield();
}

void
usage(void)
{
	fprint(2, "usage: drawterm [-a authserver] [-c cpuserver] [-s secstore] [-u user]\n");
	exits("usage");
}
int fdd;

int
mountfactotum(void)
{
	int fd;
	
	if((fd = dialfactotum()) < 0)
		return -1;
	if(sysmount(fd, -1, "/mnt/factotum", MREPL, "") < 0){
		fprint(2, "mount factotum: %r\n");
		return -1;
	}
	if((fd = open("/mnt/factotum/ctl", OREAD)) < 0){
		fprint(2, "open /mnt/factotum/ctl: %r\n");
		return -1;
	}
	close(fd);
	return 0;
}

void
rcpu(char *host)
{
	static char script[] = 
"mount -nc /fd/0 /mnt/term || exit	\n"
"bind -q /mnt/term/dev/cons /dev/cons	\n"
"</dev/cons >/dev/cons >[2=1] {		\n"
"	service=cpu exec rc -li		\n"
"}					\n";
	AuthInfo *ai;
	TLSconn *conn;
	char *na;
	int fd;

	na = netmkaddr(host, "tcp", "17019");
	if((fd = dial(na, 0, 0, 0)) < 0)
		return;

	ai = p9any(fd);
	if(ai == nil)
		fatal(1, "can't authenticate");

	conn = mallocz(sizeof(TLSconn), 1);
	conn->pskID = "p9secret";
	conn->psk = ai->secret;
	conn->psklen = ai->nsecret;

	fd = tlsClient(fd, conn);
	if(fd < 0)
		fatal(1, "tlsClient");

	auth_freeAI(ai);

	if(fprint(fd, "%7ld\n%s", strlen(script), script) < 0)
		fatal(1, "sending script");

	/* Begin serving the namespace */
	exportfs(fd, msgsize);
	fatal(1, "starting exportfs");
}

void
cpumain(int argc, char **argv)
{
	char dat[MaxStr], buf[MaxStr], cmd[MaxStr], *err, *secstoreserver, *p, *s;
	int fd, ms, data;

	/* see if we should use a larger message size */
	fd = open("/dev/draw", OREAD);
	if(fd > 0){
		ms = iounit(fd);
		if(msgsize < ms+IOHDRSZ)
			msgsize = ms+IOHDRSZ;
		close(fd);
	}

	user = getenv("USER");
	secstoreserver = nil;
	authserver = getenv("auth");
	system = getenv("cpu");
	ARGBEGIN{
	case 'a':
		authserver = EARGF(usage());
		break;
	case 'c':
		system = EARGF(usage());
		break;
	case 'd':
		dbg++;
		break;
	case 'e':
		ealgs = EARGF(usage());
		if(*ealgs == 0 || strcmp(ealgs, "clear") == 0)
			ealgs = nil;
		break;
	case 'C':
		cflag++;
		cmd[0] = '!';
		cmd[1] = '\0';
		while((p = ARGF()) != nil) {
			strcat(cmd, " ");
			strcat(cmd, p);
		}
		break;
	case 'k':
		keyspec = EARGF(usage());
		break;
	case 'r':
		base = EARGF(usage());
		break;
	case 's':
		secstoreserver = EARGF(usage());
		break;
	case 'u':
		user = EARGF(usage());
		break;
	default:
		usage();
	}ARGEND;

	if(argc != 0)
		usage();

	if(system == nil)
		system = readcons("cpu", "cpu", 0);

	if(user == nil)
		user = readcons("user", "glenda", 0);

	if(authserver == nil)
		authserver = readcons("auth", system, 0);

	if(mountfactotum() < 0){
		if(secstoreserver == nil)
			secstoreserver = authserver;
	 	if(havesecstore(secstoreserver, user)){
			s = secstorefetch(secstoreserver, user, nil);
			if(s){
				if(strlen(s) >= sizeof secstorebuf)
					sysfatal("secstore data too big");
				strcpy(secstorebuf, s);
			}
		}
	}

	rcpu(system);

	if((err = rexcall(&data, system, srvname)))
		fatal(1, "%s: %s", err, system);

	/* Tell the remote side the command to execute and where our working directory is */
	if(cflag)
		writestr(data, cmd, "command", 0);
	if(getcwd(dat, sizeof(dat)) == 0)
		writestr(data, "NO", "dir", 0);
	else
		writestr(data, dat, "dir", 0);

	/* 
	 *  Wait for the other end to execute and start our file service
	 *  of /mnt/term
	 */
	if(readstr(data, buf, sizeof(buf)) < 0)
		fatal(1, "waiting for FS: %r");
	if(strncmp("FS", buf, 2) != 0) {
		print("remote cpu: %s", buf);
		exits(buf);
	}

	if(readstr(data, buf, sizeof buf) < 0)
		fatal(1, "waiting for remote export: %r");
	if(strcmp(buf, "/") != 0){
		print("remote cpu: %s" , buf);
		exits(buf);
	}
	write(data, "OK", 2);

	/* Begin serving the gnot namespace */
	exportfs(data, msgsize);
	fatal(1, "starting exportfs");
}

void
fatal(int syserr, char *fmt, ...)
{
	Fmt f;
	char *str;
	va_list arg;

	fmtstrinit(&f);
	fmtprint(&f, "cpu: ");
	va_start(arg, fmt);
	fmtvprint(&f, fmt, arg);
	va_end(arg);
	if(syserr)
		fmtprint(&f, ": %r");
	fmtprint(&f, "\n");
	str = fmtstrflush(&f);
	write(2, str, strlen(str));
	exits(str);
}

char *negstr = "negotiating authentication method";

char bug[256];

char*
rexcall(int *fd, char *host, char *service)
{
	char *na;
	char dir[MaxStr];
	char err[ERRMAX];
	char msg[MaxStr];
	int n;

	na = netmkaddr(host, "tcp", "17010");
	if((*fd = dial(na, 0, dir, 0)) < 0)
		return "can't dial";

	/* negotiate authentication mechanism */
	if(ealgs != nil)
		snprint(msg, sizeof(msg), "%s %s", am->name, ealgs);
	else
		snprint(msg, sizeof(msg), "%s", am->name);
	writestr(*fd, msg, negstr, 0);
	n = readstr(*fd, err, sizeof err);
	if(n < 0)
		return negstr;
	if(*err){
		werrstr(err);
		return negstr;
	}

	/* authenticate */
	*fd = (*am->cf)(*fd);
	if(*fd < 0)
		return "can't authenticate";
	return 0;
}

void
writestr(int fd, char *str, char *thing, int ignore)
{
	int l, n;

	l = strlen(str);
	n = write(fd, str, l+1);
	if(!ignore && n < 0)
		fatal(1, "writing network: %s", thing);
}

int
readstr(int fd, char *str, int len)
{
	int n;

	while(len) {
		n = read(fd, str, 1);
		if(n < 0) 
			return -1;
		if(*str == '\0')
			return 0;
		str++;
		len--;
	}
	return -1;
}

static void
mksecret(char *t, uchar *f)
{
	sprint(t, "%2.2ux%2.2ux%2.2ux%2.2ux%2.2ux%2.2ux%2.2ux%2.2ux%2.2ux%2.2ux",
		f[0], f[1], f[2], f[3], f[4], f[5], f[6], f[7], f[8], f[9]);
}

/*
 *  plan9 authentication followed by rc4 encryption
 */
static int
p9auth(int fd)
{
	uchar key[16];
	uchar digest[SHA1dlen];
	char fromclientsecret[21];
	char fromserversecret[21];
	int i;
	AuthInfo *ai;

	ai = p9any(fd);
	if(ai == nil)
		return -1;
	if(ealgs == nil)
		return fd;

	if(ai->nsecret < 8){
		werrstr("secret too small");
		return -1;
	}
	memmove(key+4, ai->secret, 8);

	/* exchange random numbers */
	genrandom(key, 4);
	if(write(fd, key, 4) != 4)
		return -1;
	if(readn(fd, key+12, 4) != 4)
		return -1;

	/* scramble into two secrets */
	sha1(key, sizeof(key), digest, nil);
	mksecret(fromclientsecret, digest);
	mksecret(fromserversecret, digest+10);

	/* set up encryption */
	i = pushssl(fd, ealgs, fromclientsecret, fromserversecret, nil);
	if(i < 0)
		werrstr("can't establish ssl connection: %r");
	return i;
}

int
authdial(char *net, char *dom)
{
	int fd;
	fd = dial(netmkaddr(authserver, "tcp", "567"), 0, 0, 0);
	//print("authdial %d\n", fd);
	return fd;
}

static int
getastickets(Authkey *key, Ticketreq *tr, uchar *y, char *tbuf, int tbuflen)
{
	int asfd, rv;
	char *dom;

	dom = tr->authdom;
	asfd = authdial(nil, dom);
	if(asfd < 0)
		return -1;
	if(y != nil){
		PAKpriv p;

		rv = -1;
		tr->type = AuthPAK;
		if(_asrequest(asfd, tr) != 0 || write(asfd, y, PAKYLEN) != PAKYLEN)
			goto Out;

		authpak_new(&p, key, (uchar*)tbuf, 1);
		if(write(asfd, tbuf, PAKYLEN) != PAKYLEN)
			goto Out;

		if(_asrdresp(asfd, tbuf, 2*PAKYLEN) != 2*PAKYLEN)
			goto Out;
	
		memmove(y, tbuf, PAKYLEN);
		if(authpak_finish(&p, key, (uchar*)tbuf+PAKYLEN))
			goto Out;
	}
	tr->type = AuthTreq;
	rv = _asgetticket(asfd, tr, tbuf, tbuflen);
Out:
	close(asfd);
	return rv;
}

static int
mkservertickets(Authkey *key, Ticketreq *tr, uchar *y, char *tbuf, int tbuflen)
{
	Ticket t;
	int ret;

	if(strcmp(tr->authid, tr->hostid) != 0)
		return -1;
	memset(&t, 0, sizeof(t));
	ret = 0;
	if(y != nil){
		PAKpriv p;

		t.form = 1;
		memmove(tbuf, y, PAKYLEN);
		authpak_new(&p, key, y, 0);
		authpak_finish(&p, key, (uchar*)tbuf);
	}
	memmove(t.chal, tr->chal, CHALLEN);
	strcpy(t.cuid, tr->uid);
	strcpy(t.suid, tr->uid);
	genrandom((uchar*)t.key, sizeof(t.key));
	t.num = AuthTc;
	ret += convT2M(&t, tbuf+ret, tbuflen-ret, key);
	t.num = AuthTs;
	ret += convT2M(&t, tbuf+ret, tbuflen-ret, key);
	memset(&t, 0, sizeof(t));

	return ret;
}

static int
gettickets(Authkey *key, Ticketreq *tr, uchar *y, char *tbuf, int tbuflen)
{
	int ret;
	ret = getastickets(key, tr, y, tbuf, tbuflen);
	if(ret > 0)
		return ret;
	return mkservertickets(key, tr, y, tbuf, tbuflen);
}

/*
 *  prompt user for a key.  don't care about memory leaks, runs standalone
 */
static Attr*
promptforkey(char *params)
{
	char *v;
	int fd;
	Attr *a, *attr;
	char *def;

	fd = open("/dev/cons", ORDWR);
	if(fd < 0)
		sysfatal("opening /dev/cons: %r");

	attr = _parseattr(params);
	fprint(fd, "\n!Adding key:");
	for(a=attr; a; a=a->next)
		if(a->type != AttrQuery && a->name[0] != '!')
			fprint(fd, " %q=%q", a->name, a->val);
	fprint(fd, "\n");

	for(a=attr; a; a=a->next){
		v = a->name;
		if(a->type != AttrQuery || v[0]=='!')
			continue;
		def = nil;
		if(strcmp(v, "user") == 0)
			def = getuser();
		a->val = readcons(v, def, 0);
		if(a->val == nil)
			sysfatal("user terminated key input");
		a->type = AttrNameval;
	}
	for(a=attr; a; a=a->next){
		v = a->name;
		if(a->type != AttrQuery || v[0]!='!')
			continue;
		def = nil;
		if(strcmp(v+1, "user") == 0)
			def = getuser();
		a->val = readcons(v+1, def, 1);
		if(a->val == nil)
			sysfatal("user terminated key input");
		a->type = AttrNameval;
	}
	fprint(fd, "!\n");
	close(fd);
	return attr;
}

/*
 *  send a key to the mounted factotum
 */
static int
sendkey(Attr *attr)
{
	int fd, rv;
	char buf[1024];

	fd = open("/mnt/factotum/ctl", ORDWR);
	if(fd < 0)
		sysfatal("opening /mnt/factotum/ctl: %r");
	rv = fprint(fd, "key %A\n", attr);
	read(fd, buf, sizeof buf);
	close(fd);
	return rv;
}

int
askuser(char *params)
{
	Attr *attr;
	
	fmtinstall('A', _attrfmt);
	
	attr = promptforkey(params);
	if(attr == nil)
		sysfatal("no key supplied");
	if(sendkey(attr) < 0)
		sysfatal("sending key to factotum: %r");
	return 0;
}

AuthInfo*
p9anyfactotum(int fd, int afd)
{
	return auth_proxy(fd, askuser, "proto=p9any role=client %s", keyspec);
}

AuthInfo*
p9any(int fd)
{
	char buf[1024], buf2[1024], *bbuf, *p, *proto, *dom, *u;
	char *pass;
	uchar crand[2*NONCELEN], cchal[CHALLEN], y[PAKYLEN];
	char tbuf[2*MAXTICKETLEN+MAXAUTHENTLEN+PAKYLEN], trbuf[TICKREQLEN+PAKYLEN];
	Authkey authkey;
	Authenticator auth;
	int afd, i, n, m, v2, dp9ik;
	Ticketreq tr;
	Ticket t;
	AuthInfo *ai;

	if((afd = open("/mnt/factotum/ctl", ORDWR)) >= 0)
		return p9anyfactotum(fd, afd);
	werrstr("");

	if(readstr(fd, buf, sizeof buf) < 0)
		fatal(1, "cannot read p9any negotiation");
	bbuf = buf;
	v2 = 0;
	if(strncmp(buf, "v.2 ", 4) == 0){
		v2 = 1;
		bbuf += 4;
	}
	dp9ik = 0;
	proto = nil;
	while(bbuf != nil){
		if((p = strchr(bbuf, ' ')))
			*p++ = 0;
		if((dom = strchr(bbuf, '@')) == nil)
			fatal(1, "bad p9any domain");
		*dom++ = 0;
		if(strcmp(bbuf, "p9sk1") == 0 || strcmp(bbuf, "dp9ik") == 0){
			proto = bbuf;
			if(strcmp(proto, "dp9ik") == 0){
				dp9ik = 1;
				break;
			}
		}
		bbuf = p;
	}
	if(proto == nil)
		fatal(1, "server did not offer p9sk1 or dp9ik");
	proto = estrdup(proto);
	sprint(buf2, "%s %s", proto, dom);
	if(write(fd, buf2, strlen(buf2)+1) != strlen(buf2)+1)
		fatal(1, "cannot write user/domain choice in p9any");
	if(v2){
		if(readstr(fd, buf, sizeof buf) < 0)
			fatal(1, "cannot read OK in p9any: got %d %s", n, buf);
		if(memcmp(buf, "OK\0", 3) != 0)
			fatal(1, "did not get OK in p9any");
	}
	genrandom(crand, 2*NONCELEN);
	genrandom(cchal, CHALLEN);
	if(write(fd, cchal, CHALLEN) != CHALLEN)
		fatal(1, "cannot write p9sk1 challenge");

	n = TICKREQLEN;
	if(dp9ik)
		n += PAKYLEN;

	if(readn(fd, trbuf, n) != n || convM2TR(trbuf, TICKREQLEN, &tr) <= 0)
		fatal(1, "cannot read ticket request in p9sk1");

	u = user;
	pass = findkey(&u, tr.authdom, proto);
	if(pass == nil)
	again:
		pass = getkey(u, tr.authdom, proto);
	if(pass == nil)
		fatal(1, "no password");

	passtokey(&authkey, pass);
	memset(pass, 0, strlen(pass));
	free(pass);

	strecpy(tr.hostid, tr.hostid+sizeof tr.hostid, u);
	strecpy(tr.uid, tr.uid+sizeof tr.uid, u);

	if(dp9ik){
		authpak_hash(&authkey, tr.hostid);
		memmove(y, trbuf+TICKREQLEN, PAKYLEN);
		n = gettickets(&authkey, &tr, y, tbuf, sizeof(tbuf));
	} else {
		n = gettickets(&authkey, &tr, nil, tbuf, sizeof(tbuf));
	}
	if(n <= 0)
		fatal(1, "cannot get auth tickets in p9sk1");

	m = convM2T(tbuf, n, &t, &authkey);
	if(m <= 0 || t.num != AuthTc){
		print("?password mismatch with auth server\n");
		goto again;
	}
	n -= m;
	memmove(tbuf, tbuf+m, n);

	if(dp9ik && write(fd, y, PAKYLEN) != PAKYLEN)
		fatal(1, "cannot send authpak public key back");

	auth.num = AuthAc;
	memmove(auth.rand, crand, NONCELEN);
	memmove(auth.chal, tr.chal, CHALLEN);
	m = convA2M(&auth, tbuf+n, sizeof(tbuf)-n, &t);
	n += m;

	if(write(fd, tbuf, n) != n)
		fatal(1, "cannot send ticket and authenticator back");

	if((n=readn(fd, tbuf, m)) != m || memcmp(tbuf, "cpu:", 4) == 0){
		if(n <= 4)
			fatal(1, "cannot read authenticator");

		/*
		 * didn't send back authenticator:
		 * sent back fatal error message.
		 */
		memmove(buf, tbuf, n);
		i = readn(fd, buf+n, sizeof buf-n-1);
		if(i > 0)
			n += i;
		buf[n] = 0;
		werrstr("");
		fatal(0, "server says: %s", buf);
	}
	
	if(convM2A(tbuf, n, &auth, &t) <= 0
	|| auth.num != AuthAs || tsmemcmp(auth.chal, cchal, CHALLEN) != 0){
		print("?you and auth server agree about password.\n");
		print("?server is confused.\n");
		fatal(0, "server lies got %llux want %llux", *(vlong*)auth.chal, *(vlong*)cchal);
	}
	memmove(crand+NONCELEN, auth.rand, NONCELEN);

	// print("i am %s there.\n", t.suid);

	ai = mallocz(sizeof(AuthInfo), 1);
	ai->suid = estrdup(t.suid);
	ai->cuid = estrdup(t.cuid);
	if(dp9ik){
		static char info[] = "Plan 9 session secret";
		ai->nsecret = 256;
		ai->secret = mallocz(ai->nsecret, 1);
		hkdf_x(	crand, 2*NONCELEN,
			(uchar*)info, sizeof(info)-1,
			(uchar*)t.key, NONCELEN,
			ai->secret, ai->nsecret,
			hmac_sha2_256, SHA2_256dlen);
	} else {
		ai->nsecret = 8;
		ai->secret = mallocz(ai->nsecret, 1);
		des56to64((uchar*)t.key, ai->secret);
	}

	memset(&t, 0, sizeof(t));
	memset(&auth, 0, sizeof(auth));
	memset(&authkey, 0, sizeof(authkey));
	memset(cchal, 0, sizeof(cchal));
	memset(crand, 0, sizeof(crand));
	free(proto);

	return ai;
}

static int
srvp9auth(int fd, char *user)
{
	return -1;
}

/*
 *  set authentication mechanism
 */
int
setam(char *name)
{
	for(am = authmethod; am->name != nil; am++)
		if(strcmp(am->name, name) == 0)
			return 0;
	am = authmethod;
	return -1;
}
