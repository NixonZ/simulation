/*
            _           __                _      _____ _____ _____
  ___ _   _| |__   ___ / _|_ __ ___  __ _| | __ |___  |___  |___  |
 / __| | | | '_ \ / _ \ |_| '__/ _ \/ _` | |/ /    / /   / /   / /
| (__| |_| | |_) |  __/  _| | |  __/ (_| |   <    / /   / /   / /
 \___|\__,_|_.__/ \___|_| |_|  \___|\__,_|_|\_\  /_/   /_/   /_/

*/
#include<bits/stdc++.h>
#include <ext/pb_ds/assoc_container.hpp>
using namespace __gnu_pbds;
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
const long mxN =5;
float L = 0.13 ;
float lambda(float t){
  if(t<300.00)
    return 0.3/10.0;
  else if (t<300+5*60)
    return (0.3+(1.0/(5.0*60.0))*(t-300.00))/10.00 ;
  else
    return (1.3-(1.0/(14.0*60.0))*(t-600))/10.00 ;
}
float Y(){
  float U = random ;
  return -log(U)/0.1 ;
}
float Ts_generator(float s){
  float t =s  ;
  while(1){
    float U= random ;
    t-=log(U)/L ;
    U =random ;
    if(U<=lambda(t)/L)
      return t ;
  }
}
int C(int t){
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
signed main() {
  ios_base::sync_with_stdio(false);
  cin.tie(NULL);
  //cout << setprecision(20) << fixed ;
  srand((unsigned) time(NULL)) ;
  float t = 0 ,ta= Ts_generator(0);
  vector<int> server_list(mxN,-1);
  vector<int>cumulative_departures(mxN,0);
  vector<float>td(mxN,INF) ;float T = 1440 ;
  int Na =0,n=0,c=C(t);
  for(int j=0;j<c;j++)
    server_list[j]=0 ;

  int b = 0;
  while(b<50)
  {
    cout<<"----System Status----" <<endl;
    cout<<"Present time:"<<t <<endl;
    cout<<"Number of People Present:" <<n <<endl;
    cout<<"Cummulative Number of Arrivals by t:" <<Na <<endl;
    cout<<"Time for next arrival:" <<ta <<endl;
    cout<<"Number of Servers:" << c <<endl;
    cout<<"Server List:" <<"[";
    for(int &x :server_list)
      cout<<x <<',';
    cout<<"\b]"<<endl;
    cout<<"Departure times:" <<"[";
    for(float &x :td)
      cout<<x <<',';
    cout<<"\b]"<<endl;
    float tk= (float)INT_MAX ;
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

    if(C(t)>c)
    // Server Adding
    {
      int count_disconnected=0 ;
      for(auto x:server_list)
        count_disconnected+=(x==-1) ;
      if(count_disconnected>=C(t)-c)
      { int curr = C(t)-c ;
        for(int i=0 ;curr;i++)
        {
          if(server_list[i]==-1)
          {
            --curr;
            server_list[i]=0 ;
          }
        }
      }
      else
      {
        for(int &x :server_list)
          if(x==-1)
            x=0 ;
        int curr = C(t)-c- count_disconnected ;
        for(int i=0;curr;i++)
        {
          if(server_list[i]==2)
          {
            --curr ;
            server_list[i]=0 ;
          }
        }
      }
    }
    if(C(t)<c)
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
      if(connected_servers.size()>=c-C(t))
      {
        int servers_to_close  =c-C(t) ;
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
        int cur = c-C(t)-(int)connected_servers.size() ;
        for(int &x : connected_empty)
          if(cur)
          {
            server_list[x]=-1 ;
            --cur ;
          }
      }
    }

    c=C(t) ;

    if(min(tk,ta)==ta)
    {
      cout<<"---Arrival happening---" <<endl;
      ++n ;++Na ;ta = Ts_generator(t) ;
      vector<int> empty_servers ;
      for(int i=0;i<mxN;i++)
        if(server_list[i]==0)
          empty_servers.pb(i) ;
      if(empty_servers.size())
      {
        server_list[empty_servers[0]]=1 ;
        td[empty_servers[0]]=t+Y() ;
      }
    }

    else if(min(tk,ta)==tk)
    {
      cout<<"---Departure happening at server : " <<k <<"---"<<endl;
      --n ;cumulative_departures[k]++ ;
      tk=INF ;
      if(server_list[k]==2)
        server_list[k]=-1;
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

        int Q = n- disconnected_filled.size()-connected.size() ;

        if(Q>0)
        {
          server_list[k]=1 ;
          td[k]=t+Y() ;
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
	return 0;
}
