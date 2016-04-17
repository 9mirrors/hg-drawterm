#define	ROUND(s, sz)	(((s)+((sz)-1))&~((sz)-1))

void		accounttime(void);
void		addclock0link(void (*)(void), int);
int		addphysseg(Physseg*);
void		addbootfile(char*, uchar*, ulong);
Block*		adjustblock(Block*, int);
void		alarmkproc(void*);
Block*		allocb(int);
int		anyhigher(void);
int		anyready(void);
Page*		auxpage(void);
Block*		bl2mem(uchar*, Block*, int);
int		blocklen(Block*);
void		callwithureg(void(*)(Ureg*));
char*		chanpath(Chan*);
int		cangetc(void*);
int		canlock(Lock*);
int		canpage(Proc*);
int		canputc(void*);
int		canqlock(QLock*);
int		canrlock(RWlock*);
void		chandevinit(void);
void		chandevreset(void);
void		chandevshutdown(void);
void		chanfree(Chan*);
void		chanrec(Mnt*);
void		checkalarms(void);
void		checkb(Block*, char*);
void		cinit(void);
Chan*		cclone(Chan*);
void		cclose(Chan*);
char*	clipread(void);
int		clipwrite(char*);
void		closeegrp(Egrp*);
void		closefgrp(Fgrp*);
void		closemount(Mount*);
void		closepgrp(Pgrp*);
void		closergrp(Rgrp*);
long		clrfpintr(void);
void		cmderror(Cmdbuf*, char*);
int		cmount(Chan**, Chan*, int, char*);
void		cnameclose(Cname*);
void		confinit(void);
void		confinit1(int);
int		consactive(void);
extern void		(*consdebug)(void);
void		copen(Chan*);
Block*		concatblock(Block*);
Block*		copyblock(Block*, int);
void		copypage(Page*, Page*);
int		cread(Chan*, uchar*, int, vlong);
void		cunmount(Chan*, Chan*);
void		cupdate(Chan*, uchar*, int, vlong);
void		cwrite(Chan*, uchar*, int, vlong);
ulong		dbgpc(Proc*);
int		decref(Ref*);
int		decrypt(void*, void*, int);
void		delay(int);
Chan*		devattach(int, char*);
Block*		devbread(Chan*, long, ulong);
long		devbwrite(Chan*, Block*, ulong);
Chan*		devclone(Chan*);
int		devconfig(int, char *, DevConf *);
Chan*		devcreate(Chan*, char*, int, ulong);
void		devdir(Chan*, Qid, char*, vlong, char*, long, Dir*);
long		devdirread(Chan*, char*, long, Dirtab*, int, Devgen*);
Devgen		devgen;
void		devinit(void);
int		devno(int, int);
Chan*		devopen(Chan*, int, Dirtab*, int, Devgen*);
void		devpermcheck(char*, ulong, int);
void		devpower(int);
void		devremove(Chan*);
void		devreset(void);
void		devshutdown(void);
int		devstat(Chan*, uchar*, int, Dirtab*, int, Devgen*);
Walkqid*	devwalk(Chan*, Chan*, char**, int, Dirtab*, int, Devgen*);
int		devwstat(Chan*, uchar*, int);
void		drawcmap(void);
void		dumpaproc(Proc*);
void		dumpqueues(void);
void		dumpregs(Ureg*);
void		dumpstack(void);
Fgrp*		dupfgrp(Fgrp*);
void		duppage(Page*);
void		dupswap(Page*);
int		emptystr(char*);
int		encrypt(void*, void*, int);
void		envcpy(Egrp*, Egrp*);
int		eqchan(Chan*, Chan*, int);
int		eqqid(Qid, Qid);
void		error(char*);
long		execregs(ulong, ulong, ulong);
void		exhausted(char*);
void		exit(int);
uvlong		fastticks(uvlong*);
int		fault(ulong, int);
void		fdclose(int, int);
Chan*		fdtochan(int, int, int, int);
int		fixfault(Segment*, ulong, int, int);
void		flushmmu(void);
void		forkchild(Proc*, Ureg*);
void		forkret(void);
void		free(void*);
void		freeb(Block*);
void		freeblist(Block*);
int		freebroken(void);
void		freepte(Segment*, Pte*);
void		freesegs(int);
void		freesession(Session*);
uintptr		getmalloctag(void*);
uintptr		getrealloctag(void*);
void		gotolabel(Label*);
char*		getconfenv(void);
int		haswaitq(void*);
long		hostdomainwrite(char*, int);
long		hostownerwrite(char*, int);
void		hzsched(void);
void		iallocinit(void);
Block*		iallocb(int);
void		iallocsummary(void);
long		ibrk(ulong, int);
void		ilock(Lock*);
void		iunlock(Lock*);
int		incref(Ref*);
void		initseg(void);
int		iprint(char*, ...);
void		isdir(Chan*);
int		iseve(void);
#define	islo()	(0)
Segment*	isoverlap(Proc*, ulong, int);
int		ispages(void*);
int		isphysseg(char*);
void		ixsummary(void);
void		kbdclock(void);
int		kbdcr2nl(Queue*, int);
int		kbdputc(Queue*, int);
void		kbdkey(Rune, int);
void		kbdrepeat(int);
long		keyread(char*, int, long);
void		kickpager(void);
void		killbig(void);
int		kproc(char*, void(*)(void*), void*);
void		kprocchild(Proc*, void (*)(void*), void*);
extern void		(*kproftimer)(ulong);
void		ksetenv(char*, char*, int);
void		kstrcpy(char*, char*, int);
void		kstrdup(char**, char*);
long		latin1(Rune*, int);
void		lock(Lock*);
void		lockinit(void);
void		logopen(Log*);
void		logclose(Log*);
char*		logctl(Log*, int, char**, Logflag*);
void		logn(Log*, int, void*, int);
long		logread(Log*, void*, ulong, long);
void		log(Log*, int, char*, ...);
Cmdtab*		lookupcmd(Cmdbuf*, Cmdtab*, int);
void		machinit(void);
void*		mallocz(ulong, int);
#define		malloc kmalloc
void*		malloc(ulong);
void		mallocsummary(void);
Block*		mem2bl(uchar*, int);
void		mfreeseg(Segment*, ulong, int);
void		microdelay(int);
void		mkqid(Qid*, vlong, ulong, int);
void		mmurelease(Proc*);
void		mmuswitch(Proc*);
Chan*		mntauth(Chan*, char*);
void		mntdump(void);
long		mntversion(Chan*, char*, int, int);
void		mountfree(Mount*);
ulong		ms2tk(ulong);
ulong		msize(void*);
ulong		ms2tk(ulong);
uvlong		ms2fastticks(ulong);
void		muxclose(Mnt*);
Chan*		namec(char*, int, int, ulong);
Chan*		newchan(void);
int		newfd(Chan*);
Mhead*		newmhead(Chan*);
Mount*		newmount(Mhead*, Chan*, int, char*);
Page*		newpage(int, Segment **, ulong);
Pgrp*		newpgrp(void);
Rgrp*		newrgrp(void);
Proc*		newproc(void);
char*		nextelem(char*, char*);
void		nexterror(void);
Cname*		newcname(char*);
int		notify(Ureg*);
int		nrand(int);
int		okaddr(ulong, ulong, int);
int		openmode(ulong);
void		oserrstr(void);
void		oserror(void);
Block*		packblock(Block*);
Block*		padblock(Block*, int);
void		pagechainhead(Page*);
void		pageinit(void);
void		pagersummary(void);
void		panic(char*, ...);
Cmdbuf*		parsecmd(char *a, int n);
ulong		perfticks(void);
void		pexit(char*, int);
int		preempted(void);
void		printinit(void);
int		procindex(ulong);
void		pgrpcpy(Pgrp*, Pgrp*);
void		pgrpnote(ulong, char*, long, int);
Pgrp*		pgrptab(int);
void		pio(Segment *, ulong, ulong, Page **);
#define		poperror()		up->nerrlab--
void		portclock(Ureg*);
int		postnote(Proc*, int, char*, int);
int		pprint(char*, ...);
void		prflush(void);
ulong		procalarm(ulong);
int		proccounter(char *name);
void		procctl(Proc*);
void		procdump(void);
int		procfdprint(Chan*, int, int, char*, int);
void		procinit0(void);
void		procflushseg(Segment*);
void		procpriority(Proc*, int, int);
Proc*		proctab(int);
void		procwired(Proc*, int);
Pte*		ptealloc(void);
Pte*		ptecpy(Pte*);
int		pullblock(Block**, int);
Block*		pullupblock(Block*, int);
Block*		pullupqueue(Queue*, int);
void		putmhead(Mhead*);
void		putmmu(ulong, ulong, Page*);
void		putpage(Page*);
void		putseg(Segment*);
void		putstr(char*);
void		putstrn(char*, int);
void		putswap(Page*);
ulong		pwait(Waitmsg*);
Label*	pwaserror(void);
void		qaddlist(Queue*, Block*);
Block*		qbread(Queue*, int);
long		qbwrite(Queue*, Block*);
Queue*		qbypass(void (*)(void*, Block*), void*);
int		qcanread(Queue*);
void		qclose(Queue*);
int		qconsume(Queue*, void*, int);
Block*		qcopy(Queue*, int, ulong);
int		qdiscard(Queue*, int);
void		qflush(Queue*);
void		qfree(Queue*);
int		qfull(Queue*);
Block*		qget(Queue*);
void		qhangup(Queue*, char*);
int		qisclosed(Queue*);
void		qinit(void);
int		qiwrite(Queue*, void*, int);
int		qlen(Queue*);
void		qlock(QLock*);
Queue*		qopen(int, int, void (*)(void*), void*);
int		qpass(Queue*, Block*);
int		qpassnolim(Queue*, Block*);
int		qproduce(Queue*, void*, int);
void		qputback(Queue*, Block*);
long		qread(Queue*, void*, int);
Block*		qremove(Queue*);
void		qreopen(Queue*);
void		qsetlimit(Queue*, int);
void		qunlock(QLock*);
int		qwindow(Queue*);
int		qwrite(Queue*, void*, int);
void		qnoblock(Queue*, int);
int		rand(void);
void		randominit(void);
ulong		randomread(void*, ulong);
void		rdb(void);
int		readnum(ulong, char*, ulong, ulong, int);
int		readstr(ulong, char*, ulong, char*);
void		ready(Proc*);
void		rebootcmd(int, char**);
void		reboot(void*, void*, ulong);
void		relocateseg(Segment*, ulong);
void		renameuser(char*, char*);
void		resched(char*);
void		resrcwait(char*);
int		return0(void*);
void		rlock(RWlock*);
long		rtctime(void);
void		runlock(RWlock*);
Proc*		runproc(void);
void		savefpregs(FPsave*);
extern void		(*saveintrts)(void);
void		sched(void);
void		scheddump(void);
void		schedinit(void);
extern void		(*screenputs)(char*, int);
long		seconds(void);
ulong		segattach(Proc*, ulong, char *, ulong, ulong);
void		segclock(ulong);
void		segpage(Segment*, Page*);
void		setkernur(Ureg*, Proc*);
int		setlabel(Label*);
void		setmalloctag(void*, uintptr);
void		setrealloctag(void*, uintptr);
void		setregisters(Ureg*, char*, char*, int);
void		setswapchan(Chan*);
long		showfilewrite(char*, int);
char*		skipslash(char*);
void		sleep(Rendez*, int(*)(void*), void*);
void*		smalloc(ulong);
int		splhi(void);
int		spllo(void);
void		splx(int);
void		splxpc(int);
char*		srvname(Chan*);
int		swapcount(ulong);
int		swapfull(void);
void		swapinit(void);
void		timeradd(Timer*);
void		timerdel(Timer*);
void		timersinit(void);
void		timerintr(Ureg*, uvlong);
void		timerset(uvlong);
ulong		tk2ms(ulong);
#define		TK2MS(x) ((x)*(1000/HZ))
vlong		todget(vlong*);
void		todfix(void);
void		todsetfreq(vlong);
void		todinit(void);
void		todset(vlong, vlong, int);
Block*		trimblock(Block*, int, int);
void		tsleep(Rendez*, int (*)(void*), void*, int);
int		uartctl(Uart*, char*);
int		uartgetc(void);
void		uartkick(void*);
void		uartmouse(Uart*, int (*)(Queue*, int), int);
void		uartputc(int);
void		uartputs(char*, int);
void		uartrecv(Uart*, char);
Uart*		uartsetup(Uart*);
int		uartstageoutput(Uart*);
void		unbreak(Proc*);
void		uncachepage(Page*);
long		unionread(Chan*, void*, long);
void		unlock(Lock*);
Proc**	uploc(void);
void		userinit(void);
ulong		userpc(void);
long		userwrite(char*, int);
#define	validaddr(a, b, c)
void		validname(char*, int);
void		validstat(uchar*, int);
void		vcacheinval(Page*, ulong);
void*		vmemchr(void*, int, int);
Proc*		wakeup(Rendez*);
int		walk(Chan**, char**, int, int, int*);
#define	waserror()	(setjmp(pwaserror()->buf))
void		wlock(RWlock*);
void		wunlock(RWlock*);
void*		xalloc(ulong);
void*		xallocz(ulong, int);
void		xfree(void*);
void		xhole(ulong, ulong);
void		xinit(void);
int		xmerge(void*, void*);
void*		xspanalloc(ulong, int, ulong);
void		xsummary(void);
void		osyield(void);
void		osmsleep(int);
Segment*	data2txt(Segment*);
Segment*	dupseg(Segment**, int, int);
Segment*	newseg(int, ulong, ulong);
Segment*	seg(Proc*, ulong, int);
ulong	ticks(void);
void	osproc(Proc*);
void	osnewproc(Proc*);
void	procsleep(void);
void	procwakeup(Proc*);
void	osinit(void);
void	screeninit(void);
extern	void	terminit(void);

