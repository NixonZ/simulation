#include<bits/stdc++.h>
// #include <ext/pb_ds/assoc_container.hpp>
#include <iostream>
#include <fstream>
// using namespace __gnu_pbds;
using namespace std;
typedef long long ll ;
typedef pair<int,int> pii;
typedef pair<ll,ll> pll;
typedef vector<ll> vi;
typedef vector<pll> vpll;
typedef unordered_map <ll,ll> umap ;
//#pragma GCC optimize "trapv"
#define loop(i,a,b) for(ll i=a ;i<b;i++)
#define For(i,n) for(int i=0;i<(ll)n;i++)
#define Rev(i,n) for(int i=n-1;i>=0;i--)
#define Rep(i,n) for(int i=1;i<=n;++i)
#define F first
#define S second
#define pb push_back
#define em emplace_back
#define all(v) (v).begin(),(v).end()
#define mems(x, y) memset(x, y, sizeof(x))
#define sz(v) (v).size()
#define mp(a,b) make_pair(a,b)
#define pf(n) cout<<n<<"\n"
#define pff(n) cout << n << " " ;
#define ar array
#define endl "\n"
#define PI acos(-1)
#define random (float) rand()/RAND_MAX ;
#define INF 1e6
//#define int ll
long const M=1e9+7;


const long mxN =5; // max number of Servers
float L = 0.13 ; // Max rate of arrival

float lambda(float t)
{
  if(t<300.00)
    return 0.3/10.0;
  else if (t<300+5*60)
    return (0.3+(1.0/(5.0*60.0))*(t-300.00))/10.00 ;
  else
    return (1.3-(1.0/(14.0*60.0))*(t-600))/10.00 ;
}

float Normals(float mu,float sigma)
{
  // float U1 = random ;
  // float U2 = random;
  // float V1 = 2*U1 - 1; 
  // float V2 = 2*U2 - 1;
  // float S = V1*V1 + V2*V2;
  // if(S>1)
  //   return Normals(mu,sigma);
  // pair<float,float> Normals;
  // Normals.first = (sqrt(-2.0*log(S)/S)*V1)*sigma + mu;
  // Normals.second = (sqrt(-2.0 * log(S) / S) * V1) * sigma + mu;
  // return min(Normals.first,Normals.second);
  float U = random;
  return (-log((1.0/U)-1)/1.702)*sigma+mu;
}

float DepartureTimes(float t)
{
  if (t < 300.00)
    return Normals(30,10);
  else if (t < 300 + 5 * 60)
    return Normals(10,2);
  else
    return Normals(30,10);
}

float Y()
{
  float U = random ;
  if ( U!=0 && U<0.85)
    return -log(U)/0.05 ;
  else
    return Y();
}

float Ts_generator(float s)
{
  float t = s ;
  while(1)
  {
    float U= random ;
    t-=log(U)/L ;
    U =random ;
    if (U <= lambda( (t - int(t)) + int(t) % 1440 ) / L)
      return t ;
  }
}

int C(float t)
{
    if (t<10)
      return 3;
    else if (t<120)
      return 2;
    else if (t<7*60+30)
      return 1;
    else if (t<10*60)
      return 2;
    else if (t<10*60+20)
      return 3;
    else if (t<14*60+30)
      return 4;
    else if (t<18*60 +30)
      return 5;
    else if (t<20*60)
      return 4;
    else
      return 3;
}


signed main()
{
  ios_base::sync_with_stdio(false);
  cin.tie(NULL);
  //cout << setprecision(20) << fixed ;
  srand((unsigned) time(NULL)) ;

  // Initialisation
  float t = 0 ,ta= Ts_generator(0);
  vector<int> server_list(mxN,-1);
  vector<int>cumulative_departures(mxN,0);
  vector<float>td(mxN,INF) ;float T = 1440 ;
  vector< array<float,3> > A(INF);
  vector<float> D(INF) ;
  vector<float> S(INF) ;
  vector<int>current_customer(mxN,-1) ;
  queue<int> current_queue ;
  int customer_number=15 ;
  int Na =15,n=15,c=C(t);
  for(int j=0;j<c;j++){
    server_list[j]=1 ;
    td[j]=DepartureTimes(0) ;
    current_customer[j]=j ;
  }
  for(int i=c;i<15;i++){
      current_queue.push(i) ;
 
  }

  // CSV output File
  std::ofstream data,logger;
  data.open("data.csv", std::ofstream::out);
  data << "Customer,Time of arrival,Number of people at arrival,Number of people currently in queue at arrival,Which 10 min interval in day,Which day in week,Time of start of service,Departure time,Wait time,\n";

  logger.open("log.txt",std::ofstream::out);

  int b = 0;
  while(b<10000)
  {
    cout<<"----System Status----" <<endl;
    cout<<"Present time:"<<t <<endl;
    cout<<"Number of People Present:" <<n <<endl;
    cout<<"Cummulative Number of Arrivals by t:" <<Na <<endl;
    cout<<"Time for next arrival:" <<ta <<endl;
    cout<<"Number of Servers:" << c <<endl;

    logger<<"------System Status------" <<endl;
    logger<<"Present time:"<<t <<endl;
    logger << "Number of People Present:" << n << endl;
    logger << "Cummulative Number of Arrivals by t:" << Na << endl;
    logger << "Time for next arrival:" << ta << endl;
    logger << "Number of Servers:" << c << endl;

    cout<<"Server List:" <<"[";
    for(int &x :server_list)
      cout<<x <<',';
    cout<<"\b]"<<endl;

    logger << "Server List:" << "[";
    for (int &x : server_list)
      logger << x << ',';
    logger <<']' <<endl;

    cout<<"Departure times:" <<"[";
    for(float &x :td)
      cout<<x <<',';
    cout<<"\b]"<<endl;

    logger << "Departure times:" << "[";
    for (float &x : td)
      logger << x << ',';
    logger << ']' << endl;

    cout << "Queue list: [";
    logger << "Queue list: [";
    vector<int> v;
    while (!current_queue.empty())
    {
      int x = current_queue.front();
      v.push_back(x);
      current_queue.pop();
    }
    for (auto &x : v)
    {
      cout << x << ',';
      logger <<x <<',';
      current_queue.push(x);
    }
    logger<<']' <<endl;
    cout << "\b]" << endl;

    float tk= INF  ;
    int k =-1 ;
    for(int i=0;i<mxN;i++)
    {
      if(tk>td[i])
      {
        tk=td[i] ;
        k=i ;
      }
    }

    t=min(tk,ta) ;
    float c_ = C( (t - int(t)) + int(t) % 1440 ) ;
     if (c_ > c)
    // Server Adding
    {
      int count_disconnected=0 ;
      for(auto x:server_list)
      {
        if (x==-1)
        {
          count_disconnected++;
        }
      }
      if(count_disconnected>= c_-c)
      {
        int cur = c_ - c;
        for(int i=0 ;cur>0;i++)
        {
          if(server_list[i]==-1)
          {
            --cur;
            if(!current_queue.empty())
            {
              current_customer[i]=current_queue.front() ;
              cout << "Customer " << current_queue.front() << " entering service" << endl;
              logger << "Customer " << current_queue.front() << " entering service" << endl;
              S[current_queue.front()]=t;
              current_queue.pop() ;
              server_list[i]=1 ;
              td[i] = t + DepartureTimes((t - int(t)) + int(t) % 1440);
            }
            else
            {
              current_customer[i]=-1 ;
              server_list[i]=0 ;
              
            }
          }
        }
      }
      else
      {
        // for(int &x :server_list)
        //   if(x==-1)
        //     x=0 ;
        for(int i=0;i<mxN;i++){
          if(server_list[i]==-1){
            if(!current_queue.empty()){
              current_customer[i]=current_queue.front() ;
              cout << "Customer " << current_queue.front() << " entering service" << endl;
              logger << "Customer " << current_queue.front() << " entering service" << endl;
              S[current_queue.front()] = t;
              current_queue.pop() ;
              server_list[i]=1 ;
              td[i] = t + DepartureTimes( (t - int(t)) + int(t) % 1440 );
            }
            else{
              current_customer[i]=-1 ;
              server_list[i]=0 ;
            }
          }
        }
        int curr = c_-c- count_disconnected ;
        for(int i=0;curr;i++)
        {
          if(server_list[i]==2)
          {
            --curr ;
            // server_list[i]= ;
            if(!current_queue.empty())
            {
              server_list[i]=1 ;
            }
            else
            {
              server_list[i]=0 ;
            }
          }
        }
      }
    }
    if(c_<c)
    // Server removal
    {
      vector<int> connected_servers,connected_empty;
      for(int i=0;i<mxN;i++)
      {
        if(server_list[i]==1)
          connected_servers.pb(i) ;
        if(server_list[i]==0)
          connected_empty.pb(i) ;
      }
      if(connected_servers.size()>=c-c_)
      {
        int servers_to_close  =c-c_ ;
        for(int &x: connected_servers)
        {
          if(servers_to_close)
          {
            server_list[x]=2 ;
            --servers_to_close ;
          }
          else
            break ;
        }
      }
      else
      {
        for(int &x :connected_servers)
          server_list[x]=2 ;
        int cur = c-c_-(int)connected_servers.size() ;
        for(int &x : connected_empty)
          if(cur)
          {
            server_list[x]=-1 ;
            --cur ;
          }
      }
    }

    c=c_ ;

    if(min(tk,ta)==ta)
    {
      cout<<"---Arrival happening of " <<customer_number <<" ---"<<endl;
      logger << "---Arrival happening of " << customer_number << " ---" << endl;
      ++n ;++Na ;
      ta = Ts_generator(t);
      vector<int> empty_servers ;

      for(int i=0;i<mxN;i++)
        if(server_list[i]==0)
          empty_servers.pb(i) ;

      if(empty_servers.size())
      {
        server_list[empty_servers[0]]=1 ;
        td[empty_servers[0]] = t + DepartureTimes( (t - int(t)) + int(t) % 1440 );
        current_customer[empty_servers[0]]=customer_number ;
        S[customer_number]=t;
      }
      else
        current_queue.push(customer_number) ;
      A[customer_number][0]= t;
      A[customer_number][1]= n;
      A[customer_number][2]= current_queue.size();
      ++customer_number ;
    }

    else if(min(tk,ta)==tk)
    {
      cout<<"---Departure happening at server : " <<k <<"---"<<endl;
      logger << "---Departure happening at server : " << k << "---" << endl;
      --n ;cumulative_departures[k]++ ;
      D[current_customer[k]]=t ;
      if(!current_queue.empty() && server_list[k]!=2)
      {
        assert(current_queue.front()>=0) ;
        current_customer[k]=current_queue.front() ;
        current_queue.pop() ;
      }
      else
        current_customer[k]=-1 ;
      td[k] = INF;
      if(server_list[k]==2)
      {
        server_list[k]=-1;
        td[k]=INF ;
      }
      else
      {
        assert(server_list[k]==1) ;
        server_list[k]=0 ;
        vector<int> disconnected_filled,connected ;
        for(int i=0;i<mxN;i++)
        {
          if(server_list[i]==1 )connected.pb(i) ;
          else if (server_list[i]==2) disconnected_filled.pb(i) ;
        }

        int Q = n - disconnected_filled.size()-connected.size();

        if(Q>0)
        {
          server_list[k]=1 ;
          td[k] = t + DepartureTimes((t - int(t)) + int(t) % 1440);
          cout << "Customer " << current_customer[k] << " entering service" << endl;
          logger << "Customer " << current_customer[k] << " entering service" << endl;
          S[current_customer[k]] = t;
        }
        else
        {
          assert(Q==0);
          td[k]=INF ;
        }
      }
    }
    b++;
  }
  cout<<"Writing CSV" <<endl;
  For(i,D.size()){
    data <<i <<"," << A[i][0] << "," << A[i][1] <<"," <<A[i][2] <<"," <<((int(A[i][0]))%1440)/10 <<',' <<int(A[i][0]/1440) <<',' <<S[i] <<"," <<D[i] <<"," <<(S[i]-A[i][0]) <<"," <<"\n" ;
    if (i == Na)
    {
      data.close();
      break;
    }
  }
  logger.close();
  cout << endl ;
	return 0;
}
